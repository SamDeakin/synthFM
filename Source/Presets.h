#pragma once

#include "Operator.h"

#include <cstddef>

/*
 * A simple calss to implement preset values.
 *
 * Pass the array of configurations to construct. When setPreset() is called, that numbered preset
 * will be directly set in the saved configs.
 */
class Presets {
public:
    Presets(Operator::Options (&config)[4], float (&alpha)[4][4]);
    void setPreset(int num);
private:
    Operator::Options (&savedOpConfig)[4];
    float (&savedAlpha)[4][4];
};
