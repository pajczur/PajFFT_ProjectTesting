/*
  ==============================================================================

    GraphAnalyser.h
    Created: 16 Apr 2018 11:35:58pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include "PajFFT/PajDFT.h"
#include "PajFFT/PajFFT_MixedRadix.h"
#include "PajFFT/PajFFT_Radix2.h"
#include "PajFFT/PajFFT_fRange.h"
#include "Clock.h"

//==============================================================================
/*
*/
class CalculateDTFT    : public Component, public Timer
{
public:
    CalculateDTFT();
    ~CalculateDTFT();
    
    void setInputData(AudioBuffer<float> &inp);
    void setInputData(std::vector<float> &inp);
    void setOutputData(std::vector<float> &outp);
    
    void timerCallback() override;
    void drawGraph();
    void drawGraph3();

    void paint (Graphics&) override;
    void resized() override;
    void setSampleRate(double sample_rate);
    void setNewBufSize(double new_buf_size);
    void setRadix2BuffSize(double buf_size);
    void setZoomLogar (double lowE, double topE);
    void setZoomLinear(double startTime, double endTime);
    void setLowEndIndex();

    PajFFT_MixedRadix mixedRadix_FFT;
    PajFFT_MixedRadix mixedRadix_IFFT;
    PajFFT_Radix2     radix2_FFT;
    PajFFT_Radix2     radix2_IFFT;
    PajDFT            regular_DFT;
    PajDFT            regular_IDFT;
    
    bool dataIsInUse;
    bool dataIsReadyToFFT;
    bool dataIsAfterFFT;
    bool isForward;
    
    void selectFFT(int identifier);
    void resetOutputData();
    
private:
    double wSampleRate;
    double newBufferSize;
    double radix2BuffSize;
    double nyquist;
    float maxResolution;

    double dispResol;
    double dispLogScale;
    float zero_dB; // position on the displey of 0 dB
    double dispWidth;
    float logScaleWidth1;
    float logNyquist;
    float low_End;
    float low_End_index;
    float top_End;
    float top_End_index;
    float timeStart;
    float timeEnd;
    float linearDivider;
public:
    Path fftGraph;
private:
    std::vector<float> inputD;
    AudioBuffer<float> inputData;
 public:
    int fftType=0;
    int ddd=0;
    std::vector<float>              *wOutput1;
    std::vector<float>              *wOutput2;
    std::vector<float>               outRealMixed_1;
    std::vector<float>               outRealMixed_2;
    std::vector<std::complex<float>> outCompMixed;
    std::vector<float>               outRealRadix2_1;
    std::vector<float>               outRealRadix2_2;
    std::vector<std::complex<float>> outCompRadix2;
    std::vector<float>               outRealDFT_1;
    std::vector<float>               outRealDFT_2;
    std::vector<std::complex<float>> outCompDFT;
    
    bool oneOfTwo;
    
    float timeElapsed=0.0f;
    
    int temm;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalculateDTFT)
};
