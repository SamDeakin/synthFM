/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Synth.h"
#include "Keyboard.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    /*
     * Detect the state of modifier keys.
     *
     * Shift: Hold all notes
     * Ctrl: Shift frequency one octave down
     * Alt: Shift frequency one octave up
     */
    void modifierKeysChanged(const ModifierKeys& modifiers) override;

    /*
     * For detecting when a preset key is pressed
     */
    bool keyPressed(const KeyPress& key) override;

private:
    // Lay out different parts of the ui
    void layoutOperatorRow(Rectangle<int> bounds);
    void layoutTopRow(Rectangle<int> bounds);

    float getFrequencyMultFromLabel(Synth::OpRef op);
    float getFrequencyOffsetFromLabel(Synth::OpRef op);

    int samplesPerBlock;
    float sampleRate;

    // 1 / sampleRate, for multiplication instead of division.
    float inverseSampleRate;

    bool octaveDown;
    bool octaveUp;

    // A final overall gain value applied outside of the synth
    float finalGain;

    /*
     * Used to keep track of time.
     * Essentially we increment this by samplesPerBlock every time we have to fill the buffer.
     *
     * We use an unsigned int here instead of a float specifically because it will loop back to 0 instead of
     * slowly losing precision and then crashing, although at 44100 samples/sec that will happen after 27 hours.
     * The time should be divided by the sample rate to get the time in seconds.
     */
    unsigned time;

    Synth synth;

    // Visual Components
    Label titleLabel;

    // Labels for adjusting the synth settings
    Slider alphaSliders[4][4];
    Label synthMatrixHeader;

    Slider amplitudeSliders[4];
    Label amplitudeHeader;

    Label freqMultLabels[4];
    Label freqOffsetLabels[4];
    Label freqHeader;

    Label opNumber[4];

    Slider singleGainSliders[4];
    Label singleGainHeader;

    // Labels for adjusting the envelope
    Label envelopeLabel;
    Label lengthLabel;
    Label scaleLabel;
    Label attackLabel;
    Label attackLengthValue;
    Label attackScaleValue;
    Label peakLabel;
    Slider peakValue; // TODO This should be a slider
    Label decayLabel;
    Label decayLengthValue;
    Label decayScaleValue;
    Label releaseLabel;
    Label releaseLengthValue;
    Label releaseScaleValue;
    Label sustainLabel;
    Slider sustainValue; // TODO This should be a slider

    float getEnvelopeValue(Label& valueLabel, float current);

    // Volume slider
    Slider gainSlider;
    Label gainLabel;

    // Piano gui elements
    Keyboard keyboard;

    // Buttons for selecting presets
    Label presetsLabel;
    TextButton presetsButtons[6];

    void selectPreset(int presetNum);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
