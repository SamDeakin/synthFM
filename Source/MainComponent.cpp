/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#include <iostream>

//==============================================================================
MainComponent::MainComponent() : time(0), synth()
{
    // TODO Get the current configs from synth and use them to set initial values
    // for the labels.

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            alphaSliders[i][j].setColour(Slider::textBoxOutlineColourId, Colours::blue);
            addAndMakeVisible(alphaSliders[i][j]);
        }

        amplitudeSliders[i].setColour(Slider::textBoxOutlineColourId, Colours::teal);
        addAndMakeVisible(amplitudeSliders[i]);

        freqMultLabels[i].setColour(Label::outlineColourId, Colours::blanchedalmond);
        freqMultLabels[i].setEditable(true);
        freqMultLabels[i].setText("1.0", NotificationType::dontSendNotification);
        addAndMakeVisible(freqMultLabels[i]);

        freqOffsetLabels[i].setColour(Label::outlineColourId, Colours::fuchsia);
        freqOffsetLabels[i].setEditable(true);
        freqOffsetLabels[i].setText("0.0", NotificationType::dontSendNotification);
        addAndMakeVisible(freqOffsetLabels[i]);

        addAndMakeVisible(singleGainSliders[i]);
    }

    synthMatrixHeader.setText("Synth Matrix", NotificationType::dontSendNotification);
    addAndMakeVisible(synthMatrixHeader);

    amplitudeHeader.setText("Amplitude", NotificationType::dontSendNotification);
    addAndMakeVisible(amplitudeHeader);

    freqHeader.setText("Frequency", NotificationType::dontSendNotification);
    addAndMakeVisible(freqHeader);

    singleGainHeader.setText("Operator Gain", NotificationType::dontSendNotification);
    addAndMakeVisible(singleGainHeader);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1600, 1000);

    // specify the number of input and output channels that we want to open
    setAudioChannels (0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // Just save these settings for math later.
    samplesPerBlock = samplesPerBlockExpected;
    this->sampleRate = sampleRate;
    inverseSampleRate = 1.0 / sampleRate;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    int start = bufferToFill.startSample;
    int samples = bufferToFill.numSamples;

    float* ptr = bufferToFill.buffer->getWritePointer(0, start);

    synth.fill(440.0, time, samples, inverseSampleRate, nullptr, ptr);

    // Duplicate into the other ear
    float* other = bufferToFill.buffer->getWritePointer(1, start);
    for (int i = 0; i < samples; i++) {
        ptr[i] = 0; // TODO Remove this when done the basic GUI
        other[i] = ptr[i];
    }

    time += bufferToFill.numSamples;
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    int graphRowHeight = 300;
    int pianoRowHeight = 400;
//    int settingsRowHeight = 300;

    auto graphRowArea = area.removeFromTop(graphRowHeight);
    auto pianoRowArea = area.removeFromBottom(pianoRowHeight);
    auto settingsRowArea = area;

    {
        // The frequency controls
        auto frequencyArea = settingsRowArea.removeFromLeft(150);
        freqHeader.setBounds(frequencyArea.removeFromTop(100));

        auto multArea = frequencyArea.removeFromLeft(75);
        auto offsetArea = frequencyArea;
        for (int i = 0; i < 4; i++) {
            freqMultLabels[i].setBounds(multArea.removeFromTop(50));
        }
        for (int i = 0; i < 4; i++) {
            freqOffsetLabels[i].setBounds(offsetArea.removeFromTop(50));
        }
    }

    {
        // The Amplitude Controls
        auto amplitudeArea = settingsRowArea.removeFromLeft(200);
        amplitudeHeader.setBounds(amplitudeArea.removeFromTop(100));

        for (int i = 0; i < 4; i++) {
            amplitudeSliders[i].setBounds(amplitudeArea.removeFromTop(50));
        }
    }

    {
        // The FM Matrix
        auto matrixArea = settingsRowArea.removeFromLeft(800);
        synthMatrixHeader.setBounds(matrixArea.removeFromTop(100));

        for (int i = 0; i < 4; i++) {
            auto row = matrixArea.removeFromTop(50);
            for (int j = 0; j < 4; j++) {
                alphaSliders[i][j].setBounds(row.removeFromLeft(200));
            }
        }
    }

    {
        // The Gain Controls
        auto gainArea = settingsRowArea.removeFromLeft(200);
        singleGainHeader.setBounds(gainArea.removeFromTop(100));

        for (int i = 0; i < 4; i++) {
            singleGainSliders[i].setBounds(gainArea.removeFromTop(50));
        }
    }
}
