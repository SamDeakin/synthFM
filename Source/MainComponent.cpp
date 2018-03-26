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
            alphaSliders[i][j].onValueChange = [this, i, j]() {
                this->synth.setAlpha(i, j, this->alphaSliders[i][j].getValue());
            };
        }

        amplitudeSliders[i].setColour(Slider::textBoxOutlineColourId, Colours::teal);
        addAndMakeVisible(amplitudeSliders[i]);
        amplitudeSliders[i].onValueChange = [this, i]() {
            this->synth.setAmplitude(i, this->amplitudeSliders[i].getValue());
        };

        freqMultLabels[i].setColour(Label::outlineColourId, Colours::blanchedalmond);
        freqMultLabels[i].setEditable(true);
        freqMultLabels[i].setText("1.0", NotificationType::dontSendNotification);
        addAndMakeVisible(freqMultLabels[i]);
        freqMultLabels[i].onTextChange = [this, i]() {
            this->synth.setFreqMult(i, this->getFrequencyMultFromLabel(i));
        };

        freqOffsetLabels[i].setColour(Label::outlineColourId, Colours::fuchsia);
        freqOffsetLabels[i].setEditable(true);
        freqOffsetLabels[i].setText("0.0", NotificationType::dontSendNotification);
        addAndMakeVisible(freqOffsetLabels[i]);
        freqOffsetLabels[i].onTextChange = [this, i]() {
            this->synth.setFreqOffset(i, this->getFrequencyOffsetFromLabel(i));
        };

        addAndMakeVisible(singleGainSliders[i]);
        singleGainSliders[i].onValueChange = [this, i]() {
            this->synth.setGain(i, this->singleGainSliders[i].getValue());
        };
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
#ifdef NOSOUND
        // Set both channels to zero
        // This still performs the work to create sound and just doesn't play it.
        ptr[i] = 0;
#endif // NOSOUND
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

float MainComponent::getFrequencyMultFromLabel(Synth::OpRef op) {
    std::string text = freqMultLabels[op].getText().toStdString();

    try {
        synth.setFreqMult(op, std::stof(text, nullptr));
    } catch(std::invalid_argument e) {
        freqMultLabels[op].setText(String(synth.getFreqMult(op)), NotificationType::dontSendNotification);
    }

    return synth.getFreqMult(op);
}

float MainComponent::getFrequencyOffsetFromLabel(Synth::OpRef op) {
    std::string text = freqOffsetLabels[op].getText().toStdString();

    try {
        synth.setFreqOffset(op, std::stof(text, nullptr));
    } catch(std::invalid_argument e) {
        freqOffsetLabels[op].setText(String(synth.getFreqOffset(op)), NotificationType::dontSendNotification);
    }

    return synth.getFreqOffset(op);
}
