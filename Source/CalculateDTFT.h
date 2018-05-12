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

#define MAX_FRAME_LENGTH 8192

//==============================================================================
/*
*/
class CalculateDTFT   /* : public Timer*/
{
public:
    CalculateDTFT();
    ~CalculateDTFT();
    
    void setInputData(AudioBuffer<float> &inp);
    void setInputData(std::vector<float> &inp);
    void setOutputData(std::vector<float> &outp);
    
//    void timerCallback() override;

    void fftCalc();
    
    void setNewBufSize(double new_buf_size);
    void setRadix2BuffSize(double buf_size);

    PajFFT_MixedRadix mixedRadix_FFT;
    PajFFT_MixedRadix mixedRadix_IFFT;
    PajFFT_Radix2     radix2_FFT;
    PajFFT_Radix2     radix2_IFFT;
    PajDFT            regular_DFT;
    PajDFT            regular_IDFT;
    
    void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);
    void smbFft(float *fftBuffer, long fftFrameSize, long sign);
    
    bool dataIsInUse;
    bool dataIsReadyToFFT;
    bool isForward;
    
    void selectFFT(int identifier);
    void resetOutputData();
    
private:
    double newBufferSize;
    double radix2BuffSize;

private:
    std::vector<float> inputData;
 public:
    int fftType=0;

    std::vector<float>              *wOutput;
    float wOutputF[8192];
    
    std::vector<float>               outRealMixed;
    std::vector<float>               outRealRadix2;
    std::vector<float>               outRealDFT;
    
    std::vector<std::complex<float>> outCompMixed;
    std::vector<std::complex<float>> outCompRadix2;
    std::vector<std::complex<float>> outCompRadix22;
    std::vector<std::complex<float>> outCompDFT;
    
    
    float timeElapsed=0.0f;
    float wPitchShift;
    float *inppp;
    float outpp[8192];
    PajFFT_Radix2     tempRadix2_FFT;
    PajFFT_Radix2     tempRadix2_IFFT;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalculateDTFT)
};
