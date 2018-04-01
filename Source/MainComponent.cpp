
#include "MainComponent.h"

#include <vector>

//==============================================================================
MainComponent::MainComponent() : finalGain(1.0), time(0), synth()
{
    Font titleFont = Font("Helvetica Neue", "Thin", 38);
    Font subtitleFont = Font("Helvetica Neue", "Light", 24);
    Font labelFont = Font("Helvetica Neue", "Light", 16);
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Helvetica Neue");

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            alphaSliders[i][j].setColour(Slider::textBoxOutlineColourId, Colours::blue);
            addAndMakeVisible(alphaSliders[i][j]);
            alphaSliders[i][j].setValue(synth.getAlpha(i, j));
            alphaSliders[i][j].setRange(0, 2);
            alphaSliders[i][j].setSliderStyle(Slider::SliderStyle::LinearBar);
            alphaSliders[i][j].onValueChange = [this, i, j]() {
                this->synth.setAlpha(i, j, this->alphaSliders[i][j].getValue());
            };
        }

        amplitudeSliders[i].setColour(Slider::textBoxOutlineColourId, Colours::teal);
        addAndMakeVisible(amplitudeSliders[i]);
        amplitudeSliders[i].setValue(synth.getAmplitude(i));
        amplitudeSliders[i].setRange(0, 10);
        amplitudeSliders[i].setSliderStyle(Slider::SliderStyle::LinearBar);
        amplitudeSliders[i].onValueChange = [this, i]() {
            this->synth.setAmplitude(i, this->amplitudeSliders[i].getValue());
        };

        freqMultLabels[i].setColour(Label::outlineColourId, Colours::blanchedalmond);
        freqMultLabels[i].setFont(labelFont);
        freqMultLabels[i].setEditable(true);
        addAndMakeVisible(freqMultLabels[i]);
        freqMultLabels[i].setText(String(synth.getFreqMult(i)), NotificationType::dontSendNotification);
        freqMultLabels[i].onTextChange = [this, i]() {
            this->synth.setFreqMult(i, this->getFrequencyMultFromLabel(i));
        };

        freqOffsetLabels[i].setColour(Label::outlineColourId, Colours::fuchsia);
        freqOffsetLabels[i].setFont(labelFont);
        freqOffsetLabels[i].setEditable(true);
        addAndMakeVisible(freqOffsetLabels[i]);
        freqOffsetLabels[i].setText(String(synth.getFreqOffset(i)), NotificationType::dontSendNotification);
        freqOffsetLabels[i].onTextChange = [this, i]() {
            this->synth.setFreqOffset(i, this->getFrequencyOffsetFromLabel(i));
        };
        opNumber[i].setFont(labelFont);
        opNumber[i].setText(String(i + 1), NotificationType::dontSendNotification);
        opNumber[i].setJustificationType(Justification::centredLeft);
        addAndMakeVisible(opNumber[i]);

        addAndMakeVisible(singleGainSliders[i]);
        singleGainSliders[i].setValue(synth.getGain(i));
        singleGainSliders[i].setRange(0, 2);
        singleGainSliders[i].setSliderStyle(Slider::SliderStyle::LinearBar);
        singleGainSliders[i].onValueChange = [this, i]() {
            this->synth.setGain(i, this->singleGainSliders[i].getValue());
        };
    }

    synthMatrixHeader.setFont(titleFont);
    synthMatrixHeader.setJustificationType(Justification::centredLeft);
    synthMatrixHeader.setText("Synth Matrix", NotificationType::dontSendNotification);
    addAndMakeVisible(synthMatrixHeader);

    amplitudeHeader.setFont(titleFont);
    amplitudeHeader.setJustificationType(Justification::centredLeft);
    amplitudeHeader.setText("Amplitude", NotificationType::dontSendNotification);
    addAndMakeVisible(amplitudeHeader);

    freqHeader.setFont(titleFont);
    freqHeader.setJustificationType(Justification::centredLeft);
    freqHeader.setText("Frequency", NotificationType::dontSendNotification);
    addAndMakeVisible(freqHeader);

    singleGainHeader.setFont(titleFont);
    singleGainHeader.setJustificationType(Justification::centredLeft);
    singleGainHeader.setText("Gain", NotificationType::dontSendNotification);
    addAndMakeVisible(singleGainHeader);

    addAndMakeVisible(keyboard);
    addKeyListener(&keyboard);

    gainLabel.setText("Volume", NotificationType::dontSendNotification);
    gainLabel.setJustificationType(Justification::centredLeft);
    gainLabel.setFont(titleFont);
    addAndMakeVisible(gainLabel);
    gainSlider.setValue(finalGain);
    gainSlider.setRange(0, 2);
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(gainSlider);
    gainSlider.onValueChange = [this]() {
        this->finalGain = gainSlider.getValue();
    };

    // The envelope controls
    envelopeLabel.setText("Envelope", NotificationType::dontSendNotification);
    envelopeLabel.setJustificationType(Justification::centredLeft);
    envelopeLabel.setFont(titleFont);
    addAndMakeVisible(envelopeLabel);
    lengthLabel.setText("Length", NotificationType::dontSendNotification);
    lengthLabel.setJustificationType(Justification::centredLeft);
    lengthLabel.setFont(subtitleFont);
    addAndMakeVisible(lengthLabel);
    scaleLabel.setText("Scale", NotificationType::dontSendNotification);
    scaleLabel.setJustificationType(Justification::centredLeft);
    scaleLabel.setFont(subtitleFont);
    addAndMakeVisible(scaleLabel);
    attackLabel.setText("Attack", NotificationType::dontSendNotification);
    attackLabel.setFont(subtitleFont);
    addAndMakeVisible(attackLabel);
    decayLabel.setText("Decay", NotificationType::dontSendNotification);
    decayLabel.setFont(subtitleFont);
    addAndMakeVisible(decayLabel);
    peakLabel.setText("Peak", NotificationType::dontSendNotification);
    peakLabel.setFont(subtitleFont);
    addAndMakeVisible(peakLabel);
    sustainLabel.setText("Sustain", NotificationType::dontSendNotification);
    sustainLabel.setFont(subtitleFont);
    addAndMakeVisible(sustainLabel);
    releaseLabel.setText("Release", NotificationType::dontSendNotification);
    releaseLabel.setFont(subtitleFont);
    addAndMakeVisible(releaseLabel);

    // The envelope controls that actually have controls
    attackLengthValue.setText(String(synth.getAttackLength()), NotificationType::dontSendNotification);
    attackLengthValue.setFont(labelFont);
    attackLengthValue.setEditable(true);
    attackLengthValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(attackLengthValue);
    attackLengthValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(attackLengthValue, synth.getAttackLength());
        synth.setAttackLength(newValue, sampleRate);
    };

    attackScaleValue.setText(String(synth.getAttackScale()), NotificationType::dontSendNotification);
    attackScaleValue.setFont(labelFont);
    attackScaleValue.setEditable(true);
    attackScaleValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(attackScaleValue);
    attackScaleValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(attackScaleValue, synth.getAttackScale());
        synth.setAttackScale(newValue, sampleRate);
    };

    peakValue.setValue(synth.getMaxValue());
    peakValue.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    peakValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(peakValue);
    peakValue.onValueChange = [this]() {
        synth.setMaxValue(peakValue.getValue(), sampleRate);
    };

    decayLengthValue.setText(String(synth.getDecayLength()), NotificationType::dontSendNotification);
    decayLengthValue.setFont(labelFont);
    decayLengthValue.setEditable(true);
    decayLengthValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(decayLengthValue);
    decayLengthValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(decayLengthValue, synth.getDecayLength());
        synth.setDecayLength(newValue, sampleRate);
    };

    decayScaleValue.setText(String(synth.getDecayScale()), NotificationType::dontSendNotification);
    decayScaleValue.setFont(labelFont);
    decayScaleValue.setEditable(true);
    decayScaleValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(decayScaleValue);
    decayScaleValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(decayScaleValue, synth.getDecayScale());
        synth.setDecayScale(newValue, sampleRate);
    };

    sustainValue.setValue(synth.getSustainValue());
    sustainValue.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    sustainValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(sustainValue);
    sustainValue.onValueChange = [this]() {
        synth.setSustainValue(sustainValue.getValue(), sampleRate);
    };

    releaseLengthValue.setText(String(synth.getReleaseLength()), NotificationType::dontSendNotification);
    releaseLengthValue.setFont(labelFont);
    releaseLengthValue.setEditable(true);
    releaseLengthValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(releaseLengthValue);
    releaseLengthValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(releaseLengthValue, synth.getReleaseLength());
        synth.setReleaseLength(newValue, sampleRate);
    };

    releaseScaleValue.setText(String(synth.getReleaseScale()), NotificationType::dontSendNotification);
    releaseScaleValue.setFont(labelFont);
    releaseScaleValue.setEditable(true);
    releaseScaleValue.setColour(Label::outlineColourId, Colours::white);
    addAndMakeVisible(releaseScaleValue);
    releaseScaleValue.onTextChange = [this]() {
        float newValue = getEnvelopeValue(releaseScaleValue, synth.getReleaseScale());
        synth.setReleaseScale(newValue, sampleRate);
    };

    presetsLabel.setText("Presets", NotificationType::dontSendNotification);
    presetsLabel.setFont(titleFont);
    addAndMakeVisible(presetsLabel);
    for (int i = 0; i < 6; i++) {
        presetsButtons[i].setButtonText(String(i + 1));
        presetsButtons[i].setColour(TextButton::buttonColourId, Colours::darkgrey);
        addAndMakeVisible(presetsButtons[i]);
        presetsButtons[i].onClick = [this, i]() {
            this->selectPreset(i);
        };
    }

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1700, 1000);

    // specify the number of input and output channels that we want to open
    setAudioChannels (0, 2);

    ModifierKeys m = ModifierKeys::getCurrentModifiers();
    octaveDown = m.isCtrlDown();
    octaveUp = m.isAltDown();
    synth.setHoldNotes(m.isShiftDown());
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

    synth.setSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    int start = bufferToFill.startSample;
    int samples = bufferToFill.numSamples;

    float* ptr = bufferToFill.buffer->getWritePointer(0, start);

    std::vector<float> frequencies;
    keyboard.getFrequencies(frequencies);

    // Adjust frequencies an octave up or down
    // Simple math
    if (octaveDown) {
        for (float& f : frequencies) {
            f *= 0.5;
        }
    }
    // Cancels out when both are on
    if (octaveUp) {
        for (float& f : frequencies) {
            f *= 2.0;
        }
    }

    // Ask the synth to fill the buffer with sound
    synth.fill(frequencies, time, samples, inverseSampleRate, nullptr, ptr);

    // Duplicate into the other ear
    float* other = bufferToFill.buffer->getWritePointer(1, start);
    bool empty = true;
    for (int i = 0; i < samples; i++) {
#ifdef NOSOUND
        // Set both channels to zero
        // This still performs the work to create sound and just doesn't play it.
        ptr[i] = 0;
#endif // NOSOUND
        ptr[i] *= finalGain;
        other[i] = ptr[i];
        if (other[i]) {
            empty = false;
        }
    }

    // Reset the time instead of incrementing it if the entire buffer is empty
    if (empty) {
        time = 0;
    } else {
        time += bufferToFill.numSamples;
    }
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
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // TODO Redo the UX to actually be good
    auto area = getLocalBounds();

    int graphRowHeight = 300;
    int pianoRowHeight = 400;
//    int settingsRowHeight = 300;

    auto firstRowArea = area.removeFromTop(graphRowHeight);
    auto pianoRowArea = area.removeFromBottom(pianoRowHeight);
    auto settingsRowArea = area;

    // Removing padding from the top and settings rows
    firstRowArea.removeFromLeft(20);
    firstRowArea.removeFromTop(20);
    firstRowArea.removeFromRight(20);
    firstRowArea.removeFromBottom(20);
    settingsRowArea.removeFromLeft(20);
    settingsRowArea.removeFromTop(20);
    settingsRowArea.removeFromRight(20);
    settingsRowArea.removeFromBottom(20);

    layoutTopRow(firstRowArea);
    layoutOperatorRow(settingsRowArea);

    keyboard.setBounds(pianoRowArea);
}

void MainComponent::layoutOperatorRow(Rectangle<int> bounds) {
    int elementHorizontalSpacing = 40;

    {
        // The frequency controls
        auto frequencyArea = bounds.removeFromLeft(145);
        freqHeader.setBounds(frequencyArea.removeFromTop(50));

        auto multArea = frequencyArea.removeFromLeft(70);
        frequencyArea.removeFromLeft(5);
        auto offsetArea = frequencyArea.removeFromLeft(70);

        freqMultLabels[0].setBounds(multArea.removeFromTop(45));
        multArea.removeFromTop(10);
        freqMultLabels[1].setBounds(multArea.removeFromTop(45));
        multArea.removeFromTop(10);
        freqMultLabels[2].setBounds(multArea.removeFromTop(45));
        multArea.removeFromTop(10);
        freqMultLabels[3].setBounds(multArea.removeFromTop(45));

        freqOffsetLabels[0].setBounds(offsetArea.removeFromTop(45));
        offsetArea.removeFromTop(10);
        freqOffsetLabels[1].setBounds(offsetArea.removeFromTop(45));
        offsetArea.removeFromTop(10);
        freqOffsetLabels[2].setBounds(offsetArea.removeFromTop(45));
        offsetArea.removeFromTop(10);
        freqOffsetLabels[3].setBounds(offsetArea.removeFromTop(45));
    }

    bounds.removeFromLeft(elementHorizontalSpacing);

    {
        // The Amplitude Controls
        auto amplitudeArea = bounds.removeFromLeft(220);
        amplitudeHeader.setBounds(amplitudeArea.removeFromTop(50));

        amplitudeSliders[0].setBounds(amplitudeArea.removeFromTop(45));
        amplitudeArea.removeFromTop(10);
        amplitudeSliders[1].setBounds(amplitudeArea.removeFromTop(45));
        amplitudeArea.removeFromTop(10);
        amplitudeSliders[2].setBounds(amplitudeArea.removeFromTop(45));
        amplitudeArea.removeFromTop(10);
        amplitudeSliders[3].setBounds(amplitudeArea.removeFromTop(45));
    }

    bounds.removeFromLeft(elementHorizontalSpacing);

    {
        // The FM Matrix
        auto matrixArea = bounds.removeFromLeft(895);
        synthMatrixHeader.setBounds(matrixArea.removeFromTop(50));

        for (int i = 0; i < 4; i++) {
            auto row = matrixArea.removeFromTop(45);
            for (int j = 0; j < 4; j++) {
                alphaSliders[i][j].setBounds(row.removeFromLeft(220));
                row.removeFromLeft(5);
            }
            matrixArea.removeFromTop(10);
        }
    }

    bounds.removeFromLeft(elementHorizontalSpacing);

    {
        // The Gain Controls
        auto gainArea = bounds.removeFromLeft(220);
        singleGainHeader.setBounds(gainArea.removeFromTop(50));

        singleGainSliders[0].setBounds(gainArea.removeFromTop(45));
        gainArea.removeFromTop(10);
        singleGainSliders[1].setBounds(gainArea.removeFromTop(45));
        gainArea.removeFromTop(10);
        singleGainSliders[2].setBounds(gainArea.removeFromTop(45));
        gainArea.removeFromTop(10);
        singleGainSliders[3].setBounds(gainArea.removeFromTop(45));
    }

    bounds.removeFromLeft(elementHorizontalSpacing - 10);

    {
        // The operators header on the middle left
        auto opArea = bounds.removeFromLeft(25);

        opArea.removeFromTop(50);

        opNumber[0].setBounds(opArea.removeFromTop(45));
        opArea.removeFromTop(10);
        opNumber[1].setBounds(opArea.removeFromTop(45));
        opArea.removeFromTop(10);
        opNumber[2].setBounds(opArea.removeFromTop(45));
        opArea.removeFromTop(10);
        opNumber[3].setBounds(opArea.removeFromTop(45));
    }
}

void MainComponent::layoutTopRow(Rectangle<int> bounds) {
    int elementHorizontalSpacing = 40;

    // The gain slider
    {
        auto totalGainArea = bounds.removeFromLeft(100);
        gainLabel.setBounds(totalGainArea.removeFromTop(50));
        gainSlider.setBounds(totalGainArea);
    }

    bounds.removeFromLeft(elementHorizontalSpacing);
    
    // The envelope controls
    {
        auto envelopeArea = bounds.removeFromLeft(645);

        int labelWidth = 70;
        int attackWidth = 100;
        int peakWidth = 100;
        int decayWidth = 100;
        int sustainWidth = 100;
        int releaseWidth = 100;
        int columnGap = 15;
        int rowGap = 10;
        int dataHeight = 75;

        envelopeLabel.setBounds(envelopeArea.removeFromTop(50));

        auto labelRow = envelopeArea.removeFromTop(40);
        attackLabel.setBounds(labelRow.removeFromLeft(attackWidth));
        labelRow.removeFromLeft(columnGap);
        peakLabel.setBounds(labelRow.removeFromLeft(peakWidth));
        labelRow.removeFromLeft(columnGap);
        decayLabel.setBounds(labelRow.removeFromLeft(decayWidth));
        labelRow.removeFromLeft(columnGap);
        sustainLabel.setBounds(labelRow.removeFromLeft(sustainWidth));
        labelRow.removeFromLeft(columnGap);
        releaseLabel.setBounds(labelRow.removeFromLeft(releaseWidth));
        labelRow.removeFromLeft(columnGap);
        labelRow.removeFromLeft(labelWidth);

        envelopeArea.removeFromTop(rowGap);

        auto attackColumn = envelopeArea.removeFromLeft(attackWidth);
        attackLengthValue.setBounds(attackColumn.removeFromTop(dataHeight));
        attackColumn.removeFromTop(rowGap);
        attackScaleValue.setBounds(attackColumn.removeFromTop(dataHeight));

        envelopeArea.removeFromLeft(columnGap);

        auto peakColumn = envelopeArea.removeFromLeft(peakWidth);
        peakValue.setBounds(peakColumn);

        envelopeArea.removeFromLeft(columnGap);

        auto decayColumn = envelopeArea.removeFromLeft(decayWidth);
        decayLengthValue.setBounds(decayColumn.removeFromTop(dataHeight));
        decayColumn.removeFromTop(rowGap);
        decayScaleValue.setBounds(decayColumn.removeFromTop(dataHeight));

        envelopeArea.removeFromLeft(columnGap);

        auto sustainColumn = envelopeArea.removeFromLeft(sustainWidth);
        sustainValue.setBounds(sustainColumn);

        envelopeArea.removeFromLeft(columnGap);

        auto releaseArea = envelopeArea.removeFromLeft(releaseWidth);
        releaseLengthValue.setBounds(releaseArea.removeFromTop(dataHeight));
        releaseArea.removeFromTop(rowGap);
        releaseScaleValue.setBounds(releaseArea.removeFromTop(dataHeight));

        envelopeArea.removeFromLeft(columnGap);

        auto labelColumn = envelopeArea.removeFromLeft(labelWidth);
        lengthLabel.setBounds(labelColumn.removeFromTop(dataHeight));
        labelColumn.removeFromTop(rowGap);
        scaleLabel.setBounds(labelColumn.removeFromTop(dataHeight));
    }

    bounds.removeFromLeft(elementHorizontalSpacing);

    {
        // The preset buttons
        auto presetArea = bounds.removeFromLeft(165);
        presetsLabel.setBounds(presetArea.removeFromTop(50));

        int spacing = 15;
        int buttonSize = 60;

        auto presetRow1 = presetArea.removeFromLeft(buttonSize);
        presetArea.removeFromLeft(spacing);
        auto presetRow2 = presetArea.removeFromLeft(buttonSize);

        presetsButtons[0].setBounds(presetRow1.removeFromTop(buttonSize));
        presetRow1.removeFromTop(spacing);
        presetsButtons[1].setBounds(presetRow1.removeFromTop(buttonSize));
        presetRow1.removeFromTop(spacing);
        presetsButtons[2].setBounds(presetRow1.removeFromTop(buttonSize));

        presetsButtons[3].setBounds(presetRow2.removeFromTop(buttonSize));
        presetRow2.removeFromTop(spacing);
        presetsButtons[4].setBounds(presetRow2.removeFromTop(buttonSize));
        presetRow2.removeFromTop(spacing);
        presetsButtons[5].setBounds(presetRow2.removeFromTop(buttonSize));
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

void MainComponent::modifierKeysChanged(const ModifierKeys& modifiers) {
    synth.setHoldNotes(modifiers.isShiftDown());

    if (modifiers.isCtrlDown()) {
        octaveDown = true;
    } else {
        octaveDown = false;
    }

    if (modifiers.isAltDown()) {
        octaveUp = true;
    } else {
        octaveUp = false;
    }
}

bool MainComponent::keyPressed(const KeyPress& key) {

    if (isnumber(key.getKeyCode())) {
        int num = key.getKeyCode() - '0';
        if (num > 0 && num <= 6) {
            presetsButtons[num - 1].triggerClick();
            return true;
        }
    }
    return false;
}

float MainComponent::getEnvelopeValue(Label& valueLabel, float current) {
    std::string text = valueLabel.getText().toStdString();

    float result = current;
    try {
        result = std::stof(text, nullptr);
    } catch(std::invalid_argument e) {
        valueLabel.setText(String(current), NotificationType::dontSendNotification);
    }

    return result;
}

void MainComponent::selectPreset(int presetNum) {
    synth.selectPreset(presetNum);

    // Refresh all preset ui elements
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            alphaSliders[i][j].setValue(synth.getAlpha(i, j));
        }
        freqMultLabels[i].setText(String(synth.getFreqMult(i)), NotificationType::dontSendNotification);
        freqOffsetLabels[i].setText(String(synth.getFreqOffset(i)), NotificationType::dontSendNotification);
        amplitudeSliders[i].setValue(synth.getAmplitude(i));
    }
}
