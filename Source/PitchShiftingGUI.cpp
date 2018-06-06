/*
  ==============================================================================

    PitchShiftingGUI.cpp
    Created: 27 May 2018 7:37:15pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchShiftingGUI.h"

//==============================================================================
PitchShiftingGUI::PitchShiftingGUI()
{
    addAndMakeVisible(&pitchShiftOnOff);
    pitchShiftOnOff.setToggleState(false, dontSendNotification);
    pitchShiftOnOff.setAlwaysOnTop(true);
    pitchShiftOnOff.onClick = [this] { updateToggleState(&pitchShiftOnOff); };
    
    addAndMakeVisible(&pitchShiftOnOffLabel);
    pitchShiftOnOffLabel.setText("ON/OFF", dontSendNotification);
    pitchShiftOnOffLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(&wPitchShift);
    wPitchShift.setSliderStyle(Slider::SliderStyle::LinearVertical);
    wPitchShift.setRange(-12.0, 12.0, 0.01);
    wPitchShift.setValue(0.0);
    wPitchShift.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    wPitchShiftLabel.setJustificationType(Justification::centredBottom);
    wPitchShiftLabel.attachToComponent(&wPitchShift, false);
    wPitchShift.addListener(this);
}

PitchShiftingGUI::~PitchShiftingGUI()
{
}

void PitchShiftingGUI::timerCallback()
{
    stopTimer();
    
    if(pitchShiftOnOff.getToggleState())
    {
        calculator_FFT->mixedRadix_FFT.setWindowing(true);
        calculator_FFT->radix2_FFT.setWindowing(true);
        
        fftInterface->wInverseFFT.setToggleState(true, dontSendNotification);
        fftInterface->setInverse_fft();
        fftInterface->wInverseFFT.setVisible(false);
        fftInterface->wWindowBut.setVisible(false);
        
        fftInterface->alreadyInversed.setVisible(true);
        fftInterface->alreadyWindow.setVisible(true);
        
        fftInterface->isPitchShiftON = true;
        
        
        calculator_FFT->isWindowed = true;
        calculator_FFT->isPitchON = true;
        fftInterface->refresh();
    }
    else
    {
        calculator_FFT->mixedRadix_FFT.setWindowing(fftInterface->remembereWinWasClicked);
        calculator_FFT->radix2_FFT.setWindowing(fftInterface->remembereWinWasClicked);
        
        fftInterface->wInverseFFT.setToggleState(fftInterface->rememberInvWasClicked, dontSendNotification);
        fftInterface->setInverse_fft();
        fftInterface->wInverseFFT.setVisible(true);
        fftInterface->wWindowBut.setVisible(true);
        
        fftInterface->alreadyInversed.setVisible(false);
        fftInterface->alreadyWindow.setVisible(false);
        
        //            if(fftInterface->wInverseFFT.getToggleState())
        //            else
        //                fftInterface->wWindowBut.setVisible(false);
        
        fftInterface->isPitchShiftON = false;
        
        calculator_FFT->isWindowed = fftInterface->remembereWinWasClicked;
        calculator_FFT->isPitchON = false;
        fftInterface->refresh();
    }
    
//    fftInterface->refresh();
}

void PitchShiftingGUI::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    g.setColour (Colours::white);
    g.drawText("PITCH SHIFT", 10, 5, 120, 20, Justification::centredLeft);
    g.setColour (Colours::red);
    g.drawFittedText("(first choose FFT type", 10, 20, 110, 20, Justification::centredLeft, 1);
    g.drawFittedText("and turn on INVERSE)", 10, 32, 110, 20, Justification::centredLeft, 1);
}

void PitchShiftingGUI::resized()
{
    wPitchShift.setBounds((getWidth()/2)-30, 50, 60, 195);
    pitchShiftOnOff.setBounds((getWidth()/2)-12, 260, 55, 25);
    pitchShiftOnOffLabel.setBounds((getWidth()/2)-25, 280, 50, 20);
}

void PitchShiftingGUI::sliderValueChanged (Slider *slider)
{
    if(slider == &wPitchShift)
    {
        if(fftInterface->calculator_FFT->fftType == 0   ||   !pitchShiftOnOff.getToggleState())
        {
            if(calculator_FFT->wPitchShift >= 1.0)
                wPitchShift.setValue((calculator_FFT->wPitchShift-1.0)*12.0);
            else
                wPitchShift.setValue((calculator_FFT->wPitchShift-1.0)*24.0);
        }
        else
        {
            if(wPitchShift.getValue()>=0.0)
                calculator_FFT->wPitchShift = 1.0 +   (wPitchShift.getValue() / 12.0 );
            else
                calculator_FFT->wPitchShift = 1.0 +   (wPitchShift.getValue() / 24.0 );
        }
    }
}

void PitchShiftingGUI::updateToggleState(Button* button/*, int buttonID*/)
{
    if(fftInterface->calculator_FFT->fftType == 0   ||   !fftInterface->wInverseFFT.getToggleState())
    {
        if(pitchShiftOnOff.getToggleState())
            pitchShiftOnOff.setToggleState(false, dontSendNotification);
        else
            pitchShiftOnOff.setToggleState(true, dontSendNotification);
    }
    else
    {
        fftInterface->pauseFFT(false);
        
        startTimer((calculator_FFT->timeElapsed/1000.0f)*10.0f);
        

    }
}


void PitchShiftingGUI::setReferences(CalculateDTFT &calculator, FFTInterface &fft)
{
    calculator_FFT = &calculator;
    fftInterface = &fft;
}
