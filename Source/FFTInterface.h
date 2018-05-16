/*
  ==============================================================================

    fftInterface.h
    Created: 20 Apr 2018 8:58:04pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CalculateDTFT.h"
#include "WavesGen.h"
#include "OscInterface.h"
#include "GraphAnalyser.h"
#include "AudioPlayer.h"
#include <string>

//==============================================================================
/*
*/
class FFTInterface    : public Component,
                        public Label::Listener,
                        public Slider::Listener,
                        public Timer
{
public:
    FFTInterface(AudioAppComponent *wAudioApp);
    ~FFTInterface();
    
    void timerCallback() override;
    
    void wSettings(CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph, AudioPlayer &player, WavesGen &oscill);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider *slider) override;
    void labelTextChanged (Label *labelThatHasChanged) override;
    
    void updateToggleState(Button* button, int fftIdentifier);
//    void updateToggleZeroPad(Button* button, int fftIdentifier);
    
    void setSampleRate(double sample_rate);
    
    void setOFF_fft();
    void setON_matrixfft();
//    void setON_radix2fft();
//    void setON_regular_DFT();
    void setInverse_fft();
    void setWindowing();

    void refresh();

    
    
    
    
    double            rememberedBuffer;
private:
    CalculateDTFT     *calculator_FFT;
    GraphAnalyser     *graphAnalyser;
    OscInterface      *oscPan;
    AudioAppComponent *wAudioApplication;
    AudioPlayer       *audioPlayer;
    WavesGen          *oscillator;
    int                rememberedWaveType;
    
    double            newBufferSize;
    double            wSampleRate;
    
    
    Label             fftBufSizeEdit;
    Label             fftBufSizeEditDescript;
    Rectangle<int>    fftBufSizeEditBox;
    const int         fftBufSizeEdit_ID=10;
    
    ToggleButton      selectMatrixFFT;
    const int         selectMatrixFFT_ID=1;
    
//    ToggleButton      selectRadix2FFT;
//    const int         selectRadix2FFT_ID=2;
//    
//    ToggleButton      selectRegDFT;
//    const int         selectRegDFT_ID=3;
    
    int               whatIsChanged_ID;
    
public:
    ToggleButton      wInverseFFT;
    const int         wInverse_ID = 4;
    
private:
    TextButton        turnOFF;
    const int         turnOFF_ID=0;
    int               fftSelectorButtons=1;

//    ToggleButton      zeroPadding;
//    TextEditor        zerosPaddingDescript;
//    string            setZerosInfo          (int use, int bufSize, int zero);
    
//    ToggleButton      winHann;
//    const int         winHann_ID=5;
    
    Slider            filterSetLowEnd;
    Slider            filterSetTopEnd;
    Label             filtersDescript;
    
    Slider            setPhase;
    Label             setPhaseLabel;

    
    Label             matrixDividerEdit;
    Label             matrixDividerEditDescript;
    Rectangle<int>    matrixDividerEditBox;
    const int         matrixDividerEdit_ID=11;
    
    Label             matrixSizeInfo;
    Rectangle<int>    matrixSizeInfoBox;

    string            matrixDimToString();
public:
    bool              pauseGetNextAudioBlock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTInterface)
};
