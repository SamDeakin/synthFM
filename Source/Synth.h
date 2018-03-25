#pragma once

#include <stddef.h>

#include "Operator.h"


class Synth {
public:
    // An external reference to an operator.
    typedef int OpRef;

    struct Config {
        // We only really use just over half of this.
        // Modulation weights of the operators.
        float alpha[4][4];

        // The output weights of the operators.
        float out[4];

        size_t feedbackIterations;
    };

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
    void SetOperatorConfig(OpRef op, const Operator::Options& config);
    Operator::Options getOperatorConfig(OpRef op);

    /*
     * Get and set the internal configuration
     */
    void setConfig(const Config& config);
    Config getConfig();

private:
    // The configuration values for the operators
    Operator::Options opConfigs[4];

    // The four managed operators
    Operator ops[4];

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
