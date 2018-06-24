/*
  ==============================================================================

    oscInterface.cpp
    Created: 18 Apr 2018 10:18:18pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscInterface.h"

//==============================================================================
OscInterface::OscInterface()
{
    isGraphOn = false;
    
    wPitchBand.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    wPitchBand.addListener(this);
    wPitchBand.setTextValueSuffix(" Hz");
    wPitchBand.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 25);
    addAndMakeVisible(&wPitchBand);

    
    wAmplitude.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    wAmplitude.addListener(this);
    wAmplitude.setRange(0.0, 1.0, 0.01);
    wAmplitude.setValue(0.5);
    wAmplitude.setSkewFactorFromMidPoint(0.7);
    wAmplitude.setTextValueSuffix(" Amp");
    wAmplitude.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 25);
    addAndMakeVisible(&wAmplitude);
    
    addAndMakeVisible(&selectSinWave);
    selectSinWave.setRadioGroupId(waveRadioButtons);
    selectSinWave.onClick = [this] { updateToggleState(&selectSinWave, sinIdentifier); };
    sinWaveLabel.setJustificationType(Justification::centredLeft);
    sinWaveLabel.setText("Sine", dontSendNotification);
    sinWaveLabel.attachToComponent(&selectSinWave, true);
    
    addAndMakeVisible(&selectSawWave);
    selectSawWave.setRadioGroupId(waveRadioButtons);
    selectSawWave.onClick = [this] { updateToggleState(&selectSawWave, sawIdentifier); };
    sawWaveLabel.setJustificationType(Justification::centredLeft);
    sawWaveLabel.setText("Saw", dontSendNotification);
    sawWaveLabel.attachToComponent(&selectSawWave, true);
    
    addAndMakeVisible(&selectSqrWave);
    selectSqrWave.setRadioGroupId(waveRadioButtons);
    selectSqrWave.onClick = [this] { updateToggleState(&selectSqrWave, sqrIdentifier); };
    sqrWaveLabel.setJustificationType(Justification::centredLeft);
    sqrWaveLabel.setText("Square", dontSendNotification);
    sqrWaveLabel.attachToComponent(&selectSqrWave, true);
    
    addAndMakeVisible(&selectWhiteNoise);
    selectWhiteNoise.setRadioGroupId(waveRadioButtons);
    selectWhiteNoise.onClick = [this] { updateToggleState(&selectWhiteNoise, noiIdentifier); };
    whiteNoiseLabel.setJustificationType(Justification::centredLeft);
    whiteNoiseLabel.setText("Noise", dontSendNotification);
    whiteNoiseLabel.attachToComponent(&selectWhiteNoise, true);
    
    addAndMakeVisible(&wMuteButton);
    wMuteButton.setButtonText("Mute");
    wMuteButton.setRadioGroupId(waveRadioButtons);
    wMuteButton.onClick = [this] { updateToggleState(&wMuteButton, mutIdentifier); };
}

OscInterface::~OscInterface()
{
}

void OscInterface::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.drawText("OSCILLATOR", 30, 5, 130, 20, Justification::centredLeft);
}

void OscInterface::resized()
{
    int topMargin = 20;
    int elementWidth = 80;
    int elementHeight = 30;
    int spaceBetween = 20;
    int leftMargin = 25;
    int buttonMargin = 53;
    
    wAmplitude.setBounds         (leftMargin,   topMargin+10, elementWidth, elementWidth);
    wPitchBand.setBounds         (leftMargin,   topMargin + wAmplitude.getHeight() + spaceBetween-3, elementWidth, elementWidth);
    int knobsBound = wAmplitude.getHeight() + wPitchBand.getHeight() + spaceBetween-7;
    
    selectSinWave.setBounds      (buttonMargin, topMargin + knobsBound + spaceBetween,                       25, elementHeight);
    selectSawWave.setBounds      (buttonMargin, topMargin + knobsBound + spaceBetween +    elementHeight,    25, elementHeight);
    selectSqrWave.setBounds      (buttonMargin, topMargin + knobsBound + spaceBetween + 2*(elementHeight),   25, elementHeight);
    selectWhiteNoise.setBounds   (buttonMargin, topMargin + knobsBound + spaceBetween + 3*(elementHeight),   25, elementHeight);
    wMuteButton.setBounds        (40, topMargin + knobsBound + spaceBetween + 4*(elementHeight)+5, 50, 50);
}


void OscInterface::sliderValueChanged(Slider *slider)
{
    if(slider == &wPitchBand)
    {
        oscillator->setFrequency(wPitchBand.getValue());
    }
    
    if(slider == &wAmplitude)
    {
        oscillator->setAmplitude(wAmplitude.getValue());
    }
}

void OscInterface::updateToggleState(Button* button, WaveID waveID)
{
    switch (waveID)
    {
        case mutIdentifier:
            oscillator->selectWave(0);
            break;
        case sinIdentifier:
            oscillator->selectWave(1);
            break;
        case sawIdentifier:
            oscillator->selectWave(2);
            break;
        case sqrIdentifier:
            oscillator->selectWave(3);
            break;
        case noiIdentifier:
            oscillator->selectWave(4);
            break;
            
        default:
            break;
    }
    
    if(waveID == mutIdentifier)
    {
        selectSinWave.setToggleState(false, NotificationType::dontSendNotification);
        selectSawWave.setToggleState(false, NotificationType::dontSendNotification);
        selectSqrWave.setToggleState(false, NotificationType::dontSendNotification);
        selectWhiteNoise.setToggleState(false, NotificationType::dontSendNotification);
    }
    else
    {
        if(isGraphOn)
            graphAnalyser->startTimer(40);
    }
}

void OscInterface::setSampleRate(double sampRate)
{
    wPitchBand.setRange(0, sampRate/2.0, 0.000001);
    wPitchBand.setValue(220.0);
    wPitchBand.setSkewFactorFromMidPoint(1000);
}

void OscInterface::setReferences(WavesGen &waveGenerator, CalculateDTFT &fftCalc, GraphAnalyser &gAnalyser)
{
    calculator_FFT = &fftCalc;
    oscillator = &waveGenerator;
    graphAnalyser = &gAnalyser;
}


void OscInterface::setControlsVisible(bool areVisible)
{
    if(areVisible)
    {
        wPitchBand.setVisible(true);
        wAmplitude.setVisible(true);
        selectSinWave.setVisible(true);
        selectSawWave.setVisible(true);
        selectSqrWave.setVisible(true);
        selectWhiteNoise.setVisible(true);
        wMuteButton.setVisible(true);
    }
    else
    {
        wPitchBand.setVisible(false);
        wAmplitude.setVisible(false);
        selectSinWave.setVisible(false);
        selectSawWave.setVisible(false);
        selectSqrWave.setVisible(false);
        selectWhiteNoise.setVisible(false);
        wMuteButton.setVisible(false);
    }
}
