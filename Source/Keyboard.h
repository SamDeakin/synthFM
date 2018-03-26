#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Key.h"

const int WHITE_KEYS = 17;
const int BLACK_KEYS = 12;

const float INITIAL_WHITE_FREQUENCIES[] = {
    130.813,
    146.832,
    164.814,
    174.614,
    195.998,
    220.000,
    246.942,
    261.626,
    293.665,
    329.628,
    349.228,
    391.995,
    440.000,
    493.883,
    523.251,
    587.330,
    659.255
};

const float INITIAL_BLACK_FREQUENCIES[] = {
    138.591,
    155.563,
    184.997,
    207.652,
    233.082,
    277.183,
    311.127,
    369.994,
    415.305,
    466.164,
    554.365,
    622.254
};

class Keyboard : public Component, public KeyListener {
public:
    Keyboard();
    ~Keyboard();

    // For painting the children
    void paint(Graphics& g) override;
    void resized() override;

    // For accepting key presses
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component *originatingComponent) override;

    // Fills the vector with the frequencies that should be played currently
    void getFrequencies(std::vector<float>& out);
private:
    Key* whiteKeys[WHITE_KEYS];
    Key* blackKeys[BLACK_KEYS];
};
