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
    
    void setReferences(CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph, AudioPlayer &player, WavesGen &oscill);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider *slider) override;
    void labelTextChanged (Label *labelThatHasChanged) override;
    
    void updateToggleState(Button* button, int fftIdentifier);
    
    void setSampleRate(double sample_rate);
    void setBufferSize(double buffer_size);
    
    void setOFF_fft();
    void setON_matrixfft();
    void setON_radix2fft();
    
    void setInverse_fft();

    void pauseFFT(bool pauseFALSE_ResumeTRUE);
    
    void rememberedInvPitchWin();
    void setVisibleFiltersAndBuffSize();
    void refresh();

    
    
    
    
    double            rememberedBuffer;
    CalculateDTFT     *calculator_FFT;
    int               whatIsChanged_ID;
private:
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
    
    ToggleButton      selectRadix2FFT;
    const int         selectRadix2FFT_ID=2;

    
    
public:
    ToggleButton      wInverseFFT;
    const int         wInverse_ID = 4;
    bool              rememberInvWasClicked;
    Label             alreadyInversed;
    
    TextButton        turnOFF;
private:
    const int         turnOFF_ID=0;
    int               fftSelectorButtons=1;

    ToggleButton      zeroPadding;
    const int         zeroPadding_ID=3;
    TextEditor        zerosPaddingDescript;
    string            setZerosInfo          (int use, int bufSize, int zero);
    
    const int         winHann_ID=5;
public:
    ToggleButton      wWindowBut;
    Label             alreadyWindow;
    bool              remembereWinWasClicked;
    
private:
    Slider            filterSetLowEnd;
    Slider            filterSetTopEnd;
    Label             filtersDescript;
    ToggleButton      linkFilters;
    const int         linkFilters_ID=20;
    Label             linkFiltersLabel;
    bool              areFiltersLinked;
    double            filterDiff;
    
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
    bool              isPitchShiftON;
    bool              pauseGetNextAudioBlock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTInterface)
};
