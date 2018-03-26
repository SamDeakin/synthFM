#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

class Key : public Button {
public:
    /*
     * The frequency may be changed later.
     * The initial colour is whether this is a black or white key.
     * false for black, true for white.
     */
    Key(float initialFrequency, bool keyColour);
    ~Key();

    void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
    void resized() override;

    /*
     * Get this key's current frequency
     * To save calls, this returns 0 unless the key is currently being pressed
     */
    float getFrequency();
private:
    float frequency;
    bool keyColour;
};
