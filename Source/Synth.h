#pragma once

#include <stddef.h>

#include "Operator.h"


class Synth {
public:
    // An external reference to an operator.
    typedef int OpRef;

    /*
     * TODO args?
     */
    Synth();

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
    void fill(float freq,
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
private:
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
    };

    Config config;

    /*
     *
     */
    void callOperator(OpRef opNum,
                      float freq,
                      float start,
                      size_t samples,
                      float sampleDistance,
                      float* modulation,
                      float* out);
};
