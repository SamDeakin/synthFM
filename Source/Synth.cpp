
#include "Synth.h"
#include "Operator.h"

#include <cmath>

Synth::Synth() : sampleRate(0), ops{{opConfigs[0]}, {opConfigs[1]}, {opConfigs[2]}, {opConfigs[3]}} {
    // Zero all config values
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            config.alpha[i][j] = 0;
        }

        config.out[i] = 0;

        opConfigs[i].amplitude = 1;
        opConfigs[i].freq_multiple = 0;
        opConfigs[i].freq_offset = 0;
    }
    config.feedbackIterations = 15;

    // Set some sensible defaults
    config.out[0] = 1.0;
    config.feedbackIterations = 20;

    config.opHeadroom = 3;
    config.mixHeadroom = 4;

    opConfigs[0].freq_multiple = 1.0;
    opConfigs[0].freq_offset = 0.0;

    envelope.attackLength = 0.1;
    envelope.intAttackLength = 2;
    envelope.attackScale = 100.0;
    envelope.maxValue = 1.0;
    envelope.decayLength = 0.1;
    envelope.intDecayLength = 2;
    envelope.decayScale = 10.0;
    envelope.sustainValue = 0.8;
    envelope.releaseLength = 1.0;
    envelope.intReleaseLength = 2;
    envelope.releaseScale = 10.0;

    envelope.startValues = new float[envelope.intAttackLength + envelope.intDecayLength];
    for (int i = 0; i < envelope.intAttackLength + envelope.intDecayLength; i++) {
        envelope.startValues[i] = 0;
    }
    envelope.endValues = new float[envelope.intReleaseLength];
    for (int i = 0; i < envelope.intReleaseLength; i++) {
        envelope.endValues[i] = 0;
    }
}

Synth::~Synth() {
    delete[] envelope.startValues;
    delete[] envelope.endValues;
}

void Synth::setSampleRate(float sampleRate) {
    if (sampleRate != this->sampleRate) {
        regenEnvelope(envelope, sampleRate);
    }
    this->sampleRate = sampleRate;
}

void Synth::fill(const std::vector<float>& frequencies,
                 float start,
                 size_t samples,
                 float sampleDistance,
                 float** in,
                 float* out) {
    // First add frequencies to currentlyPlaying to reset their envelopes
    for (float freq : frequencies) {
        auto it = currentlyPlaying.find(freq);
        if (it != currentlyPlaying.end()) {
            // Reinsert with the same time and a later lastHeld time
            currentlyPlaying[freq] = std::pair<size_t, size_t>(it->second.first, it->second.first + samples);
        } else {
            // Insert as just starting
            currentlyPlaying[freq] = std::pair<size_t, size_t>(0, samples);
        }
    }

    // If we are holding notes then update our current frequencies to all be held
    if (config.holdNotes) {
        for (auto& p : currentlyPlaying) {
            p.second.second += samples;
        }
    }

    for (size_t i = 0; i < samples; i++) {
        out[i] = 0;
    }

    float temp[samples];
    // We have to copy frequencies out to avoid corrupting the iterator when we remove elements
    std::unordered_map<float, std::pair<size_t, size_t>> copyMap(currentlyPlaying);
    for (auto it : copyMap) {
        float freq = it.first;
        size_t time = it.second.first;
        size_t lastHeld = it.second.second;

        fillFrequency(freq, start, samples, sampleDistance, in, temp);

        for (size_t i = 0; i < samples; i++) {
            out[i] += applyEnvelope(temp[i], time, lastHeld);
        }

        // Update time, and remove from currentlyPlaying if it has grown past the envelope
        if (float(time) - float(lastHeld) > envelope.intReleaseLength) {
            currentlyPlaying.erase(freq);
        } else {
            currentlyPlaying[freq] = std::pair<size_t, size_t>(time + samples, lastHeld);
        }
    }

    // Scale by the headroom constant
    for (size_t i = 0; i < samples; i++) {
        out[i] /= config.mixHeadroom;
    }
}

float Synth::applyEnvelope(float val, size_t time, size_t lastHeld) {
    if (time < envelope.intAttackLength + envelope.intDecayLength) {
        // Use the beginning
        return val * envelope.startValues[time];
    } else if (lastHeld >= time) {
        // During the sustain
        return val * envelope.sustainValue;
    } else if (time - lastHeld < envelope.intReleaseLength) {
        // During the release
        return val * envelope.endValues[time - lastHeld];
//        return val * 1;
    } else {
        return 0;
    }
}

void Synth::fillFrequency(float freq,
                          float start,
                          size_t samples,
                          float sampleDistance,
                          float** in,
                          float* out) {
    // Create a stack buffer for the outputs of each operator
    float intermediate[4][samples];

    // Clear the output buffer of junk
    for (size_t i = 0; i < samples; i++) {
        out[i] = 0;
    }

    // Calculate the output values in reverse order
    for (OpRef i = 3; i >= 0; i--) {
        // Calculate the input modulation
        float modulation[samples];
        for (size_t j = 0; j < samples; j++) {
            modulation[j] = 0;
        }

        for (OpRef j = 3; j > i; j--) {
            for (size_t k = 0; k < samples; k++) {
                // lol up to k
                modulation[k] += config.alpha[i][j] * intermediate[j][k];
            }
        }

        // Fill the intermediate buffer with the operator output
        callOperator(i, freq, start, samples, sampleDistance, modulation, intermediate[i]);
    }

    for (OpRef op = 0; op < 4; op++) {
        if (config.out[op] == 0) {
            continue;
        }

        // Find the absolute max
        float max = 0;
        for (size_t sample = 0; sample < samples; sample++) {
            if (intermediate[op][sample] > max) {
                max = intermediate[op][sample];
            }
            if (-intermediate[op][sample] > max) {
                max = -intermediate[op][sample];
            }
        }

        for (size_t sample = 0; sample < samples; sample++) {
            // Rescale each intermediate value such that it will be within 0..gain
            out[sample] += intermediate[op][sample] / max * config.out[op];;
        }
    }

    // Rescale the operators based on the headroom constant.
    for (size_t sample = 0; sample < samples; sample++) {
        out[sample] /= config.opHeadroom;
    }
}

void Synth::callOperator(OpRef opNum,
                         float freq,
                         float start,
                         size_t samples,
                         float sampleDistance,
                         float* modulation,
                         float* out) {
    float in[samples];

    size_t repeats = 1;
    // Our behavior is different if the operator has feedback
    if (config.alpha[opNum][opNum] != 0) {
        repeats = config.feedbackIterations;
    }

    if (modulation) {
        for (size_t i = 0; i < samples; i++) {
            in[i] = modulation[i];
        }
    } else {
        // zero out the out buffer because we will use it as the initial input for recursion.
        for (size_t i = 0; i < samples; i++) {
            in[i] = 0;
        }
    }

    Operator& op = ops[opNum];
    for (size_t iteration = 0; iteration < repeats; iteration++) {
        op.fill(freq, start, samples, sampleDistance, in, out);
        if (iteration + 1 < repeats) {
            for (int i = 0; i < samples; i++) {
                in[i] = out[i];
            }
        }
    }
}

/*
 * Get and set the configuration for a single operator
 */
void Synth::setAmplitude(OpRef op, float amp) {
    opConfigs[op].amplitude = amp;
}

float Synth::getAmplitude(OpRef op) {
    return opConfigs[op].amplitude;
}

void Synth::setFreqOffset(OpRef op, float offset) {
    opConfigs[op].freq_offset = offset;
}

float Synth::getFreqOffset(OpRef op) {
    return opConfigs[op].freq_offset;
}

void Synth::setFreqMult(OpRef op, float mult) {
    opConfigs[op].freq_multiple = mult;
}

float Synth::getFreqMult(OpRef op) {
    return opConfigs[op].freq_multiple;
}

/*
 * Get and set the internal configuration
 */
void Synth::setAlpha(OpRef x, OpRef y, float alpha) {
    config.alpha[x][y] = alpha;
}

float Synth::getAlpha(OpRef x, OpRef y) {
    return config.alpha[x][y];
}

void Synth::setGain(OpRef op, float gain) {
    config.out[op] = gain;
}

float Synth::getGain(OpRef op) {
    return config.out[op];
}

void Synth::setFeedbackIterations(size_t iters) {
    config.feedbackIterations = iters;
}

size_t Synth::getFeedbackIterations() {
    return config.feedbackIterations;
}

void Synth::setHoldNotes(bool hold) {
    config.holdNotes = hold;
}

bool Synth::getHoldNotes() {
    return config.holdNotes;
}

void Synth::setAttackLength(float len, float sampleRate) {
    Envelope e = envelope;
    e.attackLength = len;
    regenEnvelope(e, sampleRate);
}

float Synth::getAttackLength() {
    return envelope.attackLength;
}

void Synth::setAttackScale(float scale, float sampleRate) {
    Envelope e = envelope;
    e.attackScale = scale;
    regenEnvelope(e, sampleRate);
}

float Synth::getAttackScale() {
    return envelope.attackScale;
}

void Synth::setMaxValue(float maxValue, float sampleRate) {
    Envelope e = envelope;
    e.maxValue = maxValue;
    regenEnvelope(e, sampleRate);
}

float Synth::getMaxValue() {
    return envelope.maxValue;
}

void Synth::setDecayLength(float len, float sampleRate) {
    Envelope e = envelope;
    e.decayLength = len;
    regenEnvelope(e, sampleRate);
}

float Synth::getDecayLength() {
    return envelope.decayLength;
}

void Synth::setDecayScale(float scale, float sampleRate) {
    Envelope e = envelope;
    e.decayScale = scale;
    regenEnvelope(e, sampleRate);
}

float Synth::getDecayScale() {
    return envelope.decayScale;
}

void Synth::setSustainValue(float val, float sampleRate) {
    Envelope e = envelope;
    e.sustainValue = val;
    regenEnvelope(e, sampleRate);
}

float Synth::getSustainValue() {
    return envelope.sustainValue;
}

void Synth::setReleaseLength(float len, float sampleRate) {
    Envelope e = envelope;
    e.releaseLength = len;
    regenEnvelope(e, sampleRate);
}

float Synth::getReleaseLength() {
    return envelope.releaseLength;
}

void Synth::setReleaseScale(float scale, float sampleRate) {
    Envelope e = envelope;
    e.releaseScale = scale;
    regenEnvelope(e, sampleRate);
}

float Synth::getReleaseScale() {
    return envelope.releaseScale;
}

inline float attack(float time, float scale, float minValue, float maxValue, float len) {
    float amount = exp(-1/(scale * time));
    float maxInLen = exp(-1/(scale * len));

    float range = maxValue - minValue;

    return (amount / maxInLen) * range + minValue;
}

inline float decay(float time, float scale, float minValue, float maxValue, float len) {
    float amount = 1-exp(-1/(scale * time));
    float minInLen = 1-exp(-1/(scale * len));

    float range = maxValue - minValue;

    return ((amount - minInLen) / (1.0 - minInLen)) * range + minValue;
}

inline float release(float time, float scale, float minValue, float maxValue, float len) {
    return decay(time, scale, minValue, maxValue, len);
}

void Synth::regenEnvelope(Envelope e, float sampleRate) {
    e.intAttackLength = static_cast<size_t>(e.attackLength * sampleRate);
    e.intDecayLength = static_cast<size_t>(e.decayLength * sampleRate);
    e.intReleaseLength = static_cast<size_t>(e.releaseLength * sampleRate);

    e.startValues = new float[e.intAttackLength + e.intDecayLength];
    e.endValues = new float[e.intReleaseLength];

    for (size_t i = 0; i < e.intAttackLength; i++) {
        e.startValues[i] = attack(i * (1 / sampleRate),
                                  e.attackScale,
                                  0,
                                  e.maxValue,
                                  e.attackLength) * e.maxValue;
    }
    for (size_t i = e.intAttackLength; i < e.intAttackLength + e.intDecayLength; i++) {
        e.startValues[i] = decay((i - e.intAttackLength) * (1 / sampleRate),
                                 e.decayScale,
                                 e.sustainValue,
                                 e.maxValue,
                                 e.decayLength);
    }

    for (size_t i = 0; i < e.intReleaseLength; i++) {
        e.endValues[i] = release(i * (1 / sampleRate),
                                 e.releaseScale,
                                 0,
                                 e.sustainValue,
                                 e.releaseLength);
    }

    // Save the old values
    float* oldStart = envelope.startValues;
    float* oldEnd = envelope.endValues;

    envelope = e;
    delete[] oldStart;
    delete[] oldEnd;
}
