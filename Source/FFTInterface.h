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
#include "Display_Linear.h"
#include "Display_Logarithmic.h"
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
    
    void setReferences(CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph, AudioPlayer &player, WavesGen &oscill, Display_Linear &displLinea);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider *slider) override;
    void labelTextChanged (Label *labelThatHasChanged) override;
    
    enum ButtonID {
        turnOFF_ID,
        selectMatrixFFT_ID,
        selectRadix2FFT_ID,
        zeroPadding_ID,
        wInverse_ID,
        winHann_ID,
        fftBufSizeEdit_ID,
        matrixDividerEdit_ID,
        linkFilters_ID,
        rad2FIFO_ID
    };
    
    void updateToggleState(Button* button, ButtonID buttonID);
    
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
    double twoPowerToInt(double &value);
    
    
    
    
    double            rememberedBuffer;
    CalculateDTFT     *calculator_FFT;
    int               whatIsChanged_ID;
private:
    GraphAnalyser     *graphAnalyser;
    OscInterface      *oscPan;
    AudioAppComponent *wAudioApplication;
    AudioPlayer       *audioPlayer;
    WavesGen          *oscillator;
    Display_Linear    *dispLine;
    int                rememberedWaveType;
    
    double            newBufferSize;
    double            wSampleRate;
    
    
    Label             fftBufSizeEdit;
    Label             fftBufSizeEditDescript;
    Rectangle<int>    fftBufSizeEditBox;
    
    ToggleButton      selectMatrixFFT;
    
    ToggleButton      selectRadix2FFT;

    
    
public:
    ToggleButton      wInverseFFT;
    bool              rememberInvWasClicked;
    Label             alreadyInversed;
    
    TextButton        turnOFF;
private:
    int               fftRadioButtonsID=1;

    ToggleButton      zeroPadding;
    TextEditor        zerosPaddingDescript;
    string            setZerosInfo          (int use, int bufSize, int zero);
    ToggleButton      rad2FIFO;

public:
    ToggleButton      wWindowBut;
    Label             alreadyWindow;
    bool              remembereWinWasClicked;
    
private:
    Slider            filterSetLowEnd;
    Slider            filterSetTopEnd;
    Label             filtersDescript;
    ToggleButton      linkFilters;
    Label             linkFiltersLabel;
    bool              areFiltersLinked;
    double            filterDiff;
    
    Slider            setPhase;
    Label             setPhaseLabel;

    
    Label             matrixDividerEdit;
    Label             matrixDividerEditDescript;
    Rectangle<int>    matrixDividerEditBox;
    
    Label             matrixSizeInfo;
    Rectangle<int>    matrixSizeInfoBox;

    string            matrixDimToString();
    
public:
    bool              isPitchShiftON;
    bool              pauseGetNextAudioBlock;
    bool              isGraphON;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTInterface)
};
