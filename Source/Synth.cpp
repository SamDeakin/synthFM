
#include "Synth.h"
#include "Operator.h"

Synth::Synth() : ops{{opConfigs[0]}, {opConfigs[1]}, {opConfigs[2]}, {opConfigs[3]}} {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            config.alpha[i][j] = 0;
        }

        config.out[i] = 0;
    }

    config.out[0] = 1.0;
    config.feedbackIterations = 20;

    // Some basic defaults for operator 0
    opConfigs[0].amplitude = 0.8;
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
    for (OpRef i = 0; i >= 0; i--) { // TODO all 4 not just 0
        // Calculate the input modulation
        // TODO

        // Fill the intermediate buffer with the operator output
        callOperator(i, freq, start, samples, sampleDistance, nullptr, intermediate[i]);
    }

    // Mix outputs together
    // TODO
    // Just pass through op1 for now
    for (size_t i = 0; i < samples; i++) {
        out[i] = intermediate[0][i];
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

void Synth::SetOperatorConfig(OpRef op, const Operator::Options& config) {
    // Direct copy-assignment
    opConfigs[op] = config;
}

Operator::Options Synth::getOperatorConfig(OpRef op) {
    return opConfigs[op];
}

void Synth::setConfig(const Config& config) {
    this->config  = config;
}

Synth::Config Synth::getConfig() {
    return config;
}
