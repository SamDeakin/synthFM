#include "Key.h"

Key::Key(float initialFrequency, bool keyColour) : Button("Key" + String(initialFrequency)), frequency(initialFrequency), keyColour(keyColour) {
}

Key::~Key() {}

void Key::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
    if (keyColour) {
        if (isButtonDown) {
            g.fillAll(Colours::lightgrey);
        } else {
            g.fillAll(Colours::white);
        }
        g.setColour(Colours::black);
        g.drawRect(0, 0, 100, 400);
    } else {
        if (isButtonDown) {
            g.fillAll(Colours::darkgrey);
        } else {
            g.fillAll(Colours::black);
        }
        g.setColour(Colours::black);
        g.drawRect(0, 0, 70, 250);
    }
}

void Key::resized() {
    // Intentionally empty
}

float Key::getFrequency() {
    if (getState() == Button::ButtonState::buttonDown) {
        return frequency;
    }
    return 0;
}
