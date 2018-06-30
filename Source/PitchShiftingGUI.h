/*
  ==============================================================================

    PitchShiftingGUI.h
    Created: 27 May 2018 7:37:15pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CalculateDTFT.h"
#include "FFTInterface.h"

//==============================================================================
/*
*/
class PitchShiftingGUI    : public Component, private Slider::Listener, private Timer
{
public:
    PitchShiftingGUI();
    ~PitchShiftingGUI();

    void timerCallback() override;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateToggleState(Button* button);
    void sliderValueChanged (Slider *slider) override;
    
    void setReferences(CalculateDTFT &calculator, FFTInterface &fft);
    void setControlsVisible(bool isVisible);

private:
    CalculateDTFT *calculator_FFT;
    FFTInterface  *fftInterface;
    
    Slider wPitchShift;
    Label wPitchShiftLabel;
    
    ToggleButton pitchShiftOnOff;
    Label pitchShiftOnOffLabel;
    const int pitchShiftOnOff_ID=1;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShiftingGUI)
};
