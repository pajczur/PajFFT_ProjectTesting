/*
  ==============================================================================

    oscInterface.h
    Created: 18 Apr 2018 10:18:18pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WavesGen.h"
#include "CalculateDTFT.h"
#include "GraphAnalyser.h"

//==============================================================================
/*
*/
class OscInterface    : public Component,
                        public Slider::Listener
{
public:
    OscInterface();
    ~OscInterface();

    void paint (Graphics&) override;
    void resized() override;
    
    void setControlsVisible(bool areVisible);
    
    void sliderValueChanged (Slider *slider) override;
    
    enum WaveID {
        mutIdentifier,
        sinIdentifier,
        sawIdentifier,
        sqrIdentifier,
        noiIdentifier
    };
    
    void updateToggleState(Button* button, WaveID waveID);
    
    void setSampleRate(double sampRate);
    void setReferences(WavesGen &waveGenerator, CalculateDTFT &fftCalc, GraphAnalyser &gAnalyser);

private:
    Slider wPitchBand;
    Slider wAmplitude;
public:
    ToggleButton selectSinWave;
    Label sinWaveLabel;
    
    ToggleButton selectSawWave;
    Label sawWaveLabel;
    
    ToggleButton selectSqrWave;
    Label sqrWaveLabel;
    
    ToggleButton selectWhiteNoise;
    Label whiteNoiseLabel;
    
    TextButton wMuteButton;
    
    bool isGraphOn;
private:
    int waveRadioButtons=1;
    
    CalculateDTFT *calculator_FFT;
    WavesGen *oscillator;
    GraphAnalyser *graphAnalyser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscInterface)
};
