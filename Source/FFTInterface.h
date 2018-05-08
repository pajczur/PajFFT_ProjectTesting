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
#include <string>

//==============================================================================
/*
*/
class FFTInterface    : public Component,
                        public Label::Listener,
                        public Slider::Listener
{
public:
    FFTInterface(AudioAppComponent *wAudioApp);
    ~FFTInterface();
    void sliderValueChanged (Slider *slider) override;
    void labelTextChanged (Label *labelThatHasChanged) override;
    void wSettings(CalculateDTFT &graph, OscInterface &osPan);
    void setSampleRate(double sample_rate);
    
    void setOFF_fft();
    void setON_matrixfft();
    void setON_radix2fft();
    void setON_regular_DFT();
    void setInverse_fft();
    void setWindowing();

    void paint (Graphics&) override;
    void resized() override;
    void stopEverything();
    
    void updateToggleState(Button* button, int fftIdentifier);
    void updateToggleZeroPad(Button* button, int fftIdentifier);

    double bufferForFRange;
    double rememberedBuffer;
    double remembered_fRangeBuffer;
    double remembered_matrixEBuffer;
private:
    double newBufferSize;
    double wSampleRate;
    Label fftBufSizeEdit;
    Label fftBufSize;
    Rectangle<int> bufferBox;
    CalculateDTFT *calculator_FFT;
    OscInterface *oscPan;
    
    ToggleButton selectMatrixFFT;
    Label matrixFFTLabel;
    const int matrixFFT_ID=1;
    
    ToggleButton selectRadix2FFT;
    Label radix2FFTLabel;
    const int radix2FFT_ID=2;
    
    ToggleButton selectRegDFT;
    Label regDFTLabel;
    const int regDFT_ID=3;
    
    TextButton turnOFF;
    const int turnOFF_ID=0;
    int waveSelectorButtons=1;
public:
    ToggleButton wInverseFFT;
    const int wInverse_ID = 4;
private:
    ToggleButton zeroPaddingRad2;
    TextEditor zerosInfo;
    string setZerosInfo(int use, int bufSize, int zero);
    
    ToggleButton winHann;
    const int winHann_ID=5;
    
    Slider setLowEnd;
    Slider setTopEnd;
    Slider setPhase;
    Label wPhase;

    Label lowtopInfo;
    
    Label matrixSize;
    Label matrixDivider;
    Label divideClickInfo;
    Rectangle<int> matSizeBox;
    Rectangle<int> matDiviBox;
    
    string lEndText(int lEnd);
    string tEndText(int tEnd);
    string matrixDim();
    
    AudioAppComponent *wAudioApplication;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTInterface)
};
