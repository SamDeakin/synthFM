#pragma once

#include <stddef.h>
#include <unordered_map>
#include <vector>

#include "Operator.h"

class Synth {
public:
    // An external reference to an operator.
    typedef int OpRef;

    Synth();
    ~Synth();
    void setSampleRate(float sampleRate);

    /*
     * Fill the provided buffer out with the combined output of 4 operators.
     *
     * freq: The freqency to produce a tone for
     * start: The starting time for this sample
     * samples: The length of out/modulation
     * sampleDistance: The time between two samples
     * in: Input to the synthesizer. Passing nullptr is faster than filled 0s. This is expected to by a 4 x samples matrix of values, where each level can be nullptr instead of an array.
     * out: samples elements in this buffer will be set.
     */
    void fill(const std::vector<float>& frequencies,
              float start,
              size_t samples,
              float sampleDistance,
              float** in,
              float* out);

    /*
     * Get and set the configuration for a single operator
     */
    void setAmplitude(OpRef op, float amp);
    float getAmplitude(OpRef op);
    void setFreqOffset(OpRef op, float offset);
    float getFreqOffset(OpRef op);
    void setFreqMult(OpRef op, float mult);
    float getFreqMult(OpRef op);

    /*
     * Get and set the internal configuration
     */
    void setAlpha(OpRef x, OpRef y, float alpha);
    float getAlpha(OpRef x, OpRef y);
    void setGain(OpRef op, float gain);
    float getGain(OpRef op);
    void setFeedbackIterations(size_t iters);
    size_t getFeedbackIterations();
    void setHoldNotes(bool hold);
    bool getHoldNotes();

    /*
     * Get and set the envelope configuration
     */
    void setAttackLength(float len, float sampleRate);
    float getAttackLength();
    void setAttackScale(float scale, float sampleRate);
    float getAttackScale();
    void setDecayLength(float len, float sampleRate);
    float getDecayLength();
    void setDecayScale(float scale, float sampleRate);
    float getDecayScale();
    void setSustainValue(float val, float sampleRate);
    float getSustainValue();
    void setReleaseLength(float len, float sampleRate);
    float getReleaseLength();
    void setReleaseScale(float scale, float sampleRate);
    float getRelaseScale();
private:
    float sampleRate;

    // The configuration values for the operators
    Operator::Options opConfigs[4];

    // The four managed operators
    Operator ops[4];

    struct Config {
        // We only really use just over half of this.
        // Modulation weights of the operators.
        float alpha[4][4];

        // The output weights of the operators.
        float out[4];

        size_t feedbackIterations;

        // TODO Add controls
        float totalGain;

        // Scaling values to adjust the gain when mixing multiple voices
        // TODO Add controls for this
        float mixHeadroom;
        float opHeadroom;

        // Whether or not notes should be continuously held
        bool holdNotes;
    } config;

    // TODO Add gui elements for these
    struct Envelope {
        // The length of time the attack lasts for
        float attackLength;
        size_t intAttackLength;
        // The amount the attack is scaled by. Used to control how squashed the exponential is
        float attackScale;

        // The highest value achieved
        float maxValue;

        // The length of time the decay lasts for
        float decayLength;
        size_t intDecayLength;
        float decayScale;

        // The envelope of the sustain period
        float sustainValue;

        // The length of time the release lasts for
        float releaseLength;
        size_t intReleaseLength;
        float releaseScale;

        // Arrays containing the discrete value samples for envelope modification
        // These allow the values to be pre computed now instead of later
        float* startValues;
        float* endValues;
    } envelope;

    // A helper for setting the envelope
    void regenEnvelope(Envelope e, float sampleRate);

    // map: frequency to <time, last held>
    std::unordered_map<float, std::pair<size_t, size_t>> currentlyPlaying;

    float applyEnvelope(float val, size_t time, size_t lastHeld);

    // Call the operators on one frequency.
    void fillFrequency(float freq,
                       float start,
                       size_t samples,
                       float sampleDistance,
                       float** in,
                       float* out);

    // Does the work of actually calling one operator
    void callOperator(OpRef opNum,
                      float freq,
                      float start,
                      size_t samples,
                      float sampleDistance,
                      float* modulation,
                      float* out);
};
