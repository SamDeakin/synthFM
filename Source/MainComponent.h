/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Synth.h"

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

    // Callbacks for changing GUI fields
    void matrixChanged();
    void operatorSettingsChanged(Synth::OpRef op);
    void singleGainChanged();

private:
    int samplesPerBlock;
    float sampleRate;

    // 1 / sampleRate, for multiplication instead of division.
    float inverseSampleRate;

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

    // Labels for adjusting the synth settings
    Slider alphaSliders[4][4];
    Label synthMatrixHeader;

    Slider amplitudeSliders[4];
    Label amplitudeHeader;

    Label freqMultLabels[4];
    Label freqOffsetLabels[4];
    Label freqHeader;

    Slider singleGainSliders[4];
    Label singleGainHeader;

    // Labels for adjusting the envelope
    // TODO Unimplemented

    // Graph gui elements

    // Volume slider
    Slider gainSlider;
    // TODO Need label for header?

    // Piano gui elements
    // TODO Unimplemented

    // Buttons for selecting presets
    // TODO Unimplemented

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
