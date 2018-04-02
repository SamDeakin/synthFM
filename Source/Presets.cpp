
#include "Presets.h"

static const Operator::Options operatorPresets[6][4] = {
    {
        Operator::Options{1, 0, 1},
        Operator::Options{1, 0, 2},
        Operator::Options{1, 0, 4},
        Operator::Options{1, 0, 8}
    },
    {
        Operator::Options{1, 0, 1},
        Operator::Options{1, 0, 2},
        Operator::Options{1, 0, 4},
        Operator::Options{1, 0, 8}
    },
    {
        // A "Spooky" preset with a lot of feedback
        Operator::Options{1, 0, 1},
        Operator::Options{1, 0, 1}, // Tweak the output of this one too
        Operator::Options{1, 0, 1.5},
        Operator::Options{1, 0, 0.25}
    },
    {
        Operator::Options{1, 0, 1}, // Fifths
        Operator::Options{1, 0, 3},
        Operator::Options{1, 0, 4},
        Operator::Options{1, 0, 6}
    },
    {
        Operator::Options{1, 0, 1}, // Some bass
        Operator::Options{1, 0, 2},
        Operator::Options{1, 0, 6},
        Operator::Options{1, 0, 0}
    },
    {
        Operator::Options{1, 0, 1},
        Operator::Options{1, 0, 2},
        Operator::Options{1, 0, 4},
        Operator::Options{1, 0, 8}
    }
};

static const float alphaPresets[6][4][4] = {
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        // A basic preset to show how using 4 operators in series can affect the sound
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
        {0, 0, 0, 0.5}
    },
    {
        // Spooky
        {0, 1.12, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0.45},
        {0, 0, 0, 0}
    },
    {
        // Fifths
        {0, 0.5, 0, 0},
        {0, 0, 0.5, 0},
        {0, 0, 0, 0.5},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

static const int size = 6;

Presets::Presets(Operator::Options (&config)[4], float (&alpha)[4][4]) : savedOpConfig(config), savedAlpha(alpha) {
}

void Presets::setPreset(int num) {
    if (num >= 0 && num < size) {
        for (int i = 0; i < 4; i++) {
            savedOpConfig[i] = operatorPresets[num][i];
            for (int j = 0; j < 4; j++) {
                savedAlpha[i][j] = alphaPresets[num][i][j];
            }
        }
    }
}
