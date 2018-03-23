// Expose M_PI
#define _USE_MATH_DEFINES
#include <cmath>

#include "Operator.h"

Operator::Operator(const Options& options) : options(options) {
}

void Operator::fill(float freq,
                    float start,
                    size_t samples,
                    float sampleDistance,
                    float* modulation,
                    float* out) {
    // out cannot be null
    if (out == nullptr) {
        return;
    }

    float A = options.amplitude;
    float f = freq * options.freq_multiple + options.freq_offset;

    // We do this math first to avoid doing it many times in a loop
    f = f * 2.0 * M_PI;

    // We use out as a temporary buffer as well as the output.
    for (int i = 0; i < samples; i++) {
        float time = (start + float(i)) * sampleDistance; // 2pi is already added previously
        out[i] = time * f;
    }

    if (modulation) {
        // Add modulation
        for (int i = 0; i < samples; i++) {
            out[i] += modulation[i];
        }
    }

    for (int i = 0; i < samples; i++) {
        out[i] = A * sin(out[i]);
    }
}
