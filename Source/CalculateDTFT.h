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

#define MAX_FRAME_LENGTH 44100

//==============================================================================
/*
*/
class CalculateDTFT
{
public:
    CalculateDTFT();
    ~CalculateDTFT();
    
    void setInputData(AudioBuffer<float> &inp);
    void setInputData(std::vector<float> &inp);
    void setOutputData(std::vector<float> &outp);
    

    void fftCalc();
    
    void setNewBufSize(double new_buf_size);
    void setRadix2BuffSize(double buf_size);
    void setSampleRate(float sampR);

    PajFFT_MixedRadix mixedRadix_FFT;
    
//    PajFFT_Radix2     radix2_FFT;
//    PajFFT_Radix2     radix2_IFFT;
//    PajDFT            regular_DFT;
//    PajDFT            regular_IDFT;

    void smbPitchShift2(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, std::vector<std::complex<float>> indata, std::vector<float> &outdata);
    
    bool dataIsReadyToFFT;
    bool isForward;
    bool isPitchON;
    
    void selectFFT(int identifier);
    void resetOutputData();
    
    double newBufferSize;
private:
    double radix2BuffSize;

private:
    std::vector<float>               inputData;
    std::vector<std::complex<float>> inputDataC;
 public:
    int fftType=0;
    
    std::vector<float>               outRealMixed;
    std::vector<float>               outRealRadix2;
    std::vector<float>               outRealDFT;
    
    std::vector<std::complex<float>> outCompMixed;
    std::vector<std::complex<float>> outCompRadix2;
    std::vector<std::complex<float>> outCompRadix22;
    std::vector<std::complex<float>> outCompDFT;
    
    float wSampleRate;
    
    float topEnd;
    float lowEnd;
    void setLowEnd(float lowE);
    void setTopEnd(float topE);
    
    float timeElapsed=0.0f;
    float wPitchShift;
    
    int indeX;
    
    
    // ===================== //
    // ==== PITCH SHIFT ==== //
    // ===================== //
    std::vector<std::complex<float>> gInFIFO;
    std::vector<std::complex<float>> gOutFIFO;
    std::vector<std::complex<float>> gFFTworksp;
    std::vector<std::complex<float>> outPP;
    std::vector<std::complex<float>> outPP2; 
    float gLastPhase[MAX_FRAME_LENGTH/2+1];
    float gSumPhase[MAX_FRAME_LENGTH/2+1];
    float gOutputAccum[2*MAX_FRAME_LENGTH];
    float gAnaFreq[MAX_FRAME_LENGTH];
    float gAnaMagn[MAX_FRAME_LENGTH];
    float gSynFreq[MAX_FRAME_LENGTH];
    float gSynMagn[MAX_FRAME_LENGTH];
    long gRover = false, gInit = false;
    float magn, phase, window, tmp, real, imag;
    double freqPerBin, expct;
    long qpd, index, inFifoLatency, stepSize, fftFrameSize2;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalculateDTFT)
};
