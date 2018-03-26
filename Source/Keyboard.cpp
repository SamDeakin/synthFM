
#include "Keyboard.h"

Keyboard::Keyboard() {
    for (int i = 0; i < WHITE_KEYS; i++) {
        whiteKeys[i] = new Key(INITIAL_WHITE_FREQUENCIES[i], true);
        addAndMakeVisible(whiteKeys[i]);
    }
    for (int i = 0; i < BLACK_KEYS; i++) {
        blackKeys[i] = new Key(INITIAL_BLACK_FREQUENCIES[i], false);
        addAndMakeVisible(blackKeys[i]);
    }
}

Keyboard::~Keyboard() {
    for (int i = 0; i < WHITE_KEYS; i++) {
        delete whiteKeys[i];
    }
    for (int i = 0; i < BLACK_KEYS; i++) {
        delete blackKeys[i];
    }
}

void Keyboard::paint(Graphics& g) {
    g.fillAll(Colours::aliceblue);
}

void Keyboard::resized() {
    auto area = getLocalBounds();

    auto keyArea = area;
    for (int i = 0; i < WHITE_KEYS; i++) {
        whiteKeys[i]->setBounds(keyArea.removeFromLeft(100));
    }

    // The black key placement is more specific
    blackKeys[0]->setBounds(100 - 35, area.getY(), 70, 250);
    blackKeys[1]->setBounds(200 - 35, area.getY(), 70, 250);
    blackKeys[2]->setBounds(400 - 35, area.getY(), 70, 250);
    blackKeys[3]->setBounds(500 - 35, area.getY(), 70, 250);
    blackKeys[4]->setBounds(600 - 35, area.getY(), 70, 250);
    blackKeys[5]->setBounds(800 - 35, area.getY(), 70, 250);
    blackKeys[6]->setBounds(900 - 35, area.getY(), 70, 250);
    blackKeys[7]->setBounds(1100 - 35, area.getY(), 70, 250);
    blackKeys[8]->setBounds(1200 - 35, area.getY(), 70, 250);
    blackKeys[9]->setBounds(1300 - 35, area.getY(), 70, 250);
    blackKeys[10]->setBounds(1500 - 35, area.getY(), 70, 250);
    blackKeys[11]->setBounds(1600 - 35, area.getY(), 70, 250);
}

void Keyboard::getFrequencies(std::vector<float>& out) {
    for (int i = 0; i < WHITE_KEYS; i++) {
        float value = whiteKeys[i]->getFrequency();
        if (value) {
            out.push_back(value);
        }
    }
    for (int i = 0; i < BLACK_KEYS; i++) {
        float value = blackKeys[i]->getFrequency();
        if (value) {
            out.push_back(value);
        }
    }
}
