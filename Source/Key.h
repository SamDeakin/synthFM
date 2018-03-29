#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

class Key : public Button, public KeyListener {
public:
    /*
     * The frequency may be changed later.
     * The initial colour is whether this is a black or white key.
     * false for black, true for white.
     */
    Key(float initialFrequency, bool keyColour, char hotkey);
    ~Key();

    void addKey(KeyPress key);

    void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
    void resized() override;

    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component *originatingComponent) override;

    /*
     * Get this key's current frequency
     * To save calls, this returns 0 unless the key is currently being pressed
     */
    float getFrequency();
private:
    // Get the text of frequencyLabel and set frequency to it.
    void setFrequency();

    float frequency;
    bool keyColour;

    KeyPress listenedKey;

    // Labels to provide the user with more information
    Label hotkeyLabel;
    Label frequencyLabel;
};
