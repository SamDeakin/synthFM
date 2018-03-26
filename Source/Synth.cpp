
#include "Synth.h"
#include "Operator.h"

#include <cmath>

Synth::Synth() : ops{{opConfigs[0]}, {opConfigs[1]}, {opConfigs[2]}, {opConfigs[3]}} {
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
    config.out[0] = 0.4;
    config.feedbackIterations = 20;

    opConfigs[0].freq_multiple = 1.0;
    opConfigs[0].freq_offset = 0.0;
}

void Synth::fill(float freq,
                 float start,
                 size_t samples,
                 float sampleDistance,
                 float** in,
                 float* out) {
    // Create a stack buffer for the outputs of each operator
    float intermediate[4][samples];

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
            float val = intermediate[op][sample] / max * config.out[op];

            // Take the exponent to add on a log scale
            out[sample] += exp(val);
        }
    }

    // The final step of taking the log of the result to get the final scaled value
    for (size_t sample = 0; sample < samples; sample++) {
        out[sample] = log(out[sample]);
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
