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
//#include "PajFFT/PajFFT_HandyFunc.h"

//#define MAX_FRAME_LENGTH 200000

//==============================================================================
/*
*/
class CalculateDTFT
{
public:
    CalculateDTFT();
    ~CalculateDTFT();
    
    void fftCalculator(AudioBuffer<float> &inp);
    void fftCalculator(std::vector<float> &inp);
    void getInputData(AudioBuffer<float> &inp);
    

    void fftCalc();
    
    void defineDeviceBuffSize(long dev_buf_size);
    void setNewBufSize(double new_buf_size);
    void setSampleRate(double &sampR, long overLapping);

    PajFFT_MixedRadix mixedRadix_FFT;
    PajFFT_Radix2     radix2_FFT;
    
//    PajFFT_Radix2     radix2_IFFT;
//    PajDFT            regular_DFT;
//    PajDFT            regular_IDFT;
    
    void windowingOverlap_FFT();
    void inverseFFT_windowingOverlap(long &overSamp);
    void analyzeData(long &overSamp);

    void smbPitchShift(float &pitchShift, long &osamp, float &sampleRate, std::vector<std::complex<float>> &indata, std::vector<float> &outdata);
    void windowOverlap_ForwBackFFT(long &osamp, float &sampleRate, std::vector<std::complex<float>> &indata, std::vector<float> &outdata);
    void windowOverlap_ForwFFT(long &osamp, float &sampleRate, std::vector<std::complex<float>> &indata/*, std::vector<float> &outdata*/);
    
    bool dataIsReadyToFFT;
    bool isForward;
    bool isPitchON;
    bool dataIsReadyToGraph;
    
    void selectFFT(int identifier);
    void resetOutputData();
    
    double newBufferSize;
    double rad2TrueBuffSize;
private:
    long deviceBuffSize;
//    std::complex<float> (PajFFT_Radix2::*rad2WindowChooser)         (std::complex<float>, long);
 public:
    std::vector<float>               inputData;
    std::vector<float>               tempInput;
    std::vector<std::complex<float>> inputDataC;
    int fftType=0;
    double fPi;
    
    std::vector<float>               windowedBackFFTout;
    std::vector<float>               backFFTout;
    std::vector<std::complex<float>> forwFFTout;
    std::vector<float>               wOutput;
    std::vector<float>               tempOutput;
    
    std::vector<float>               freqOutput;
    
    Atomic<bool> fftIsReady;
    
    float wSampleRate;
    
    float topEnd;
    float lowEnd;
    void setLowEnd(float lowE);
    void setTopEnd(float topE);
    
    float timeElapsed=0.0f;
    float wPitchShift;
    bool isWindowed;
    
    long indexFFToutSize;
    long indexDEVbufSize;
    long dupa;
    bool dupex;
    
    
    // ===================== //
    // ==== PITCH SHIFT ==== //
    // ===================== //
    std::vector<std::complex<float>> gInFIFO;
    std::vector<std::complex<float>> gOutFIFO;
    std::vector<std::complex<float>> gFFTworksp;
    std::vector<std::complex<float>> outPP2; 

    float *gOutputAccum;

    std::vector<float> gLastPhase;
    std::vector<float> gSumPhase;
    std::vector<float> gAnaFreq;
    std::vector<float> gAnaMagn;
    
    long gInit = false;
    float magn, phase, window, tmp, real, imag;
    double freqPerBin, expct;
    long qpd, index, inFifoLatency, stepSize, fftFrameSize2;
    
    long overLap;
    long winFrameSize;
    bool isAllocated=false;
    
    
    
    int ttt=0;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalculateDTFT)
};
