#include "Key.h"

extern Colour dark;
extern Colour dark_accent;
extern Colour light_accent;
extern Colour light;

Key::Key(float initialFrequency, bool keyColour, char hotkey) : Button("Key" + String(initialFrequency)), frequency(initialFrequency), keyColour(keyColour) {
    hotkeyLabel.setText(String() + hotkey, NotificationType::dontSendNotification);
    hotkeyLabel.setFont(Font("Helvetica Neue", "Light", 16));
    hotkeyLabel.setEditable(false);
    hotkeyLabel.setJustificationType(Justification::centred);
//    hotkeyLabel.setWantsKeyboardFocus(false);
    hotkeyLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(hotkeyLabel);

    frequencyLabel.setText(String(initialFrequency), NotificationType::dontSendNotification);
    frequencyLabel.setFont(Font("Helvetica Neue", "Light", 16));
    frequencyLabel.setEditable(true);
    frequencyLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.onTextChange = [this]() {
        this->setFrequency();
    };

    if (keyColour) {
        hotkeyLabel.setColour(Label::textColourId, dark);
        frequencyLabel.setColour(Label::textColourId, dark);
    } else {
        hotkeyLabel.setColour(Label::textColourId, light);
        frequencyLabel.setColour(Label::textColourId, light);
    }
}

Key::~Key() {}

void Key::setFrequency() {
    std::string text = frequencyLabel.getText().toStdString();

    try {
        frequency = std::stof(text, nullptr);
    } catch(std::invalid_argument e) {
        // Reset the frequency if we don't find a number
        frequencyLabel.setText(String(frequency), NotificationType::dontSendNotification);
    }
}

void Key::addKey(KeyPress key) {
    listenedKey = key;
}

void Key::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
    if (keyColour) {
        if (isButtonDown) {
            g.fillAll(light_accent);
        } else {
            g.fillAll(light);
        }
        g.setColour(dark);
        g.drawRect(0, 0, 100, 400);
    } else {
        if (isButtonDown) {
            g.fillAll(dark_accent);
        } else {
            g.fillAll(dark);
        }
        g.setColour(dark);
        g.drawRect(0, 0, 70, 250);
    }
}

void Key::resized() {
    auto area = getBounds();
    area.setX(0);
    area.setY(0);

    int bottomGap = 25;
    int middleGap = 0;
    int labelHeight = 25;

    area.removeFromBottom(bottomGap);
    frequencyLabel.setBounds(area.removeFromBottom(labelHeight));
    area.removeFromBottom(middleGap);
    hotkeyLabel.setBounds(area.removeFromBottom(labelHeight));
}

bool Key::keyPressed(const KeyPress &key, Component *originatingComponent) {
    if (key == listenedKey) {
        return true;
    }
    return false;
}

bool Key::keyStateChanged(bool isKeyDown, Component *originatingComponent) {
    if (KeyPress::isKeyCurrentlyDown(listenedKey.getKeyCode())) {
        setState(ButtonState::buttonDown);
    } else {
        setState(ButtonState::buttonNormal);
    }

    return false;
}

float Key::getFrequency() {
    if (getState() == Button::ButtonState::buttonDown) {
        return frequency;
    }
    return 0;
}
