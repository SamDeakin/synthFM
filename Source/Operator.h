#pragma once

#include <stddef.h>

class Operator {
public:
    struct Options {
        // The Amplitude of the wave
        float amplitude;

        // The multiple and offset from the base frequency this operator will produce.
        float freq_offset;
        float freq_multiple;
    };

    /*
     * Create an Operator.
     *
     * options: A reference to the externally-managed Options object.
     */
    Operator(const Options& options);

    /*
     * Fill the provided buffer out with the output of this operator
     *
     * freq: The freqency to produce a tone for
     * start: The starting time for this sample
     * samples: The length of out/modulation
     * sampleDistance: The time between two samples
     * modulation: Input to this operator used to carry another operator. Passing nullptr is faster than all 0s.
     * out: samples elements in this buffer will be set.
     */
    void fill(float freq,
              float start,
              size_t samples,
              float sampleDistance,
              float* modulation,
              float* out);

private:
    const Options& options;
};
