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
    void updateToggleState(Button* button, int waveIdentifier);
    
    void settings(WavesGen &waveGenerator, CalculateDTFT &graph, double sampRate);

private:
    Slider wPitchBand;
    Slider wAmplitude;
public:
    ToggleButton selectSinWave;
    Label sinWaveLabel;
    int sinIdentifier=1;
    
    ToggleButton selectSawWave;
    Label sawWaveLabel;
    int sawIdentifier=2;
    
    ToggleButton selectSqrWave;
    Label sqrWaveLabel;
    int sqrIdentifier=3;
    
    ToggleButton selectWhiteNoise;
    Label whiteNoiseLabel;
    int noiIdentifier=4;
    
    TextButton wMuteButton;
    int mutIdentifier=0;
private:
    int waveSelectorButtons=1;
    
    CalculateDTFT *calculator_FFT;
    WavesGen *oscillator;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscInterface)
};
