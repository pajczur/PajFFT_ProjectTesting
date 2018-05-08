/*
  ==============================================================================

     WojFFT.h
     Created: 13 Mar 2018 9:11:15pm
     Author:  Wojtek Pilwinski

  ==============================================================================
  ==============================================================================
  =========================== RADIX-2 FFT ALGORITHM ============================
  ==============================================================================
  ==============================================================================
 
     DESCRIPTION:
 
     You can:
     1) Perform forward or backward (inverse) Radix-2 FFT - works best on buffer sizes = 2^L
        COMMENT:
        In the depth it works only wit buffer sizes = 2^L, but for other sizes there is autonatic convertion to size 2^L.
        And then zero padding happens or smaller buffer size than actuall.
 
     2) Change frequency range to return.
        COMMENT:
        It only returns the frequencies range you choosen. But calculations are almost the same.
        So changing range does not make calculations faster.
 
     3) Zero overloading for buffer sizes that are not 2^L
        COMMENT:
        a) if you turn off zero padding then FFT algorithm uses smaller sample rate than actuall sample rate,
           so output result is less accurate.
        b) Zero overloading works good only on forward FFT,
           For inverse it's turnd off automatically - for consequences please read point a)
 
   ==============================================================================
 
     MANUAL
     1) Declare variable of type "WojFFT"
 
     2) Call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE);" to prepare all necessary data.
 
     3) Now everything is ready to perform FFT by calling "makeFFT(std::vector<float> inputSignal);"
 
     4) To get output data just use "outputData" which is std::vector of type float, and it's public.
 
     5) You can set freq range by:
        a) setLowEnd(float lowEnd); WARNING: lowEnd can't be less than zero,   and can't be greater than topEnd.
        b) setTopEnd(float topEnd); WARNING: topEnd can't be less than lowEnd, and can't be greater than sample rate.
        SEE DESCRIPTION COMMENT FOR POINT 2)
 
     REMEMBER:
         If you don't set freq range, by default they are from zero to Nyquist freq:
         low_End = 0;
         top_End = sampleRate/2;
 
 ==============================================================================
 
     BUGS TO IMPROVE
     1) Thera are problems with amplitude of inverse DFT output.
        For buffer size equal to sample rate everything is OK,
        but for buffer size < sample rate thera are unproper results.
 
     2) Fix zero overloading for inverse FFT
 
     3) Generally optimize the code like:
        - indexes precalculations
        - iterations bounds
        - simplify the makeFFT algorithm
        - implement changing Forward/Backward after settings
          Now you can set it only by call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE)"
 
 ==============================================================================
*/

#pragma once
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>

class PajFFT_Radix2
{
    // ===========================================================================================================================================
    // == FUNCTIONS ==============================================================================================================================
    // ===========================================================================================================================================
public:
    PajFFT_Radix2();
    ~PajFFT_Radix2();
  
    
    // == SETTINGS ==================================================================
private:
    void setSampleRate            (float sampleR);
    void setBufferSize            (float bufferS);
    void resetOutputData          ();
    void updateFreqRangeScale     (float lEnd, float tEnd);
public:
    void wSettings                (float sampleRate, float bufferSize, std::vector<             float>  &wOutput , bool forwardTRUE_backwardFALSE);
    void wSettings                (float sampleRate, float bufferSize, std::vector<std::complex<float>> &wOutputC, bool forwardTRUE_backwardFALSE);
    void setLowEnd                (float lowEnd);
    void setTopEnd                (float topEnd);
    void setZeroPadding           (bool  trueON_falseOFF);
    void setPhase                 (float phase);
    void setWindowing             (bool wind);
    
    
    
    // == PRE CREATION ==============================================================
private:
    void bitReversal              (float bufSize);
    void prepareTwiddlesArray     (bool forwardOrBackward);
    void prepare_sN0_matrix       ();
    void prepareWindowingArray    ();
    
    
    
    // == F F T - RADIX 2 - ALGORITHM ===============================================
public:
    void makeFFT                  (std::vector<             float>  inputSignal);
    void makeFFT                  (std::vector<std::complex<float>> inputSignalC);
private:
    void firstStepFFT             (std::vector<             float>  inputSignal,  int fft);
    void firstStepFFTc            (std::vector<std::complex<float>> inputSignalC, int fft);
    void divideAndConquereFFT     (int fft);
    void lastStepFFT              (int fft);
    
    
    
    // == CALCULATORS ===============================================================
private:
    std::complex<float>    twiddleCalculator        (float nXk);
    void                   freqMagnitudeCalculator  (std::complex<float> fftOutput, int freqBin);
    void                   freqMagnCalc_ComplexOut  (std::complex<float> fftOutput, int freqBin);
    void                   waveAmplitudeCalculator  (std::complex<float> fftOutput, int index);
    void  (PajFFT_Radix2::*forwBackChooser)         (std::complex<float> fftOutput, int freqBinOrIndex);
    
    
    
    // == GET INFORMATIONS ==========================================================
public:
    float getBufferSize();
    float getLowEnd();
    float getTopEnd();
    float getPhase();
    
    
    
    
    // ===========================================================================================================================================
    // == VARIABLES ==============================================================================================================================
    // ===========================================================================================================================================
private:
    float fPi;
    std::complex<float> imaginary_j;
    std::complex<float> cZero;
    float fZero;
    std::complex<float> phaseRotation;
    
    float wBufferSize;
    float wSampleRate;
    float low_End;
    float top_End;
    float lEndScale;
    float tEndScale;
    int   trueBuffersize;
    bool  zeroPadding;
    bool  resizeInput;
    bool  rememberedForwardOrBackward;
    bool  dataPreparedConfirm;
    double wPhase;
    
    std::vector<std::complex<float>> wnkN;         // Array of precalculated forward twiddle - W^nk
    std::vector<int>                 bitReversed;  // Array of bit reversed indexes
    
    std::vector<std::vector<std::vector<std::complex<float>>>> sN0;  // Temporarily input complex FFT - S(n)
    std::vector<float>               windowHann;
    
    bool isWindowing;
    bool isComplexOutput;
public:
    std::vector<float> *wOutputData;
    std::vector<std::complex<float>> *wOutputDataC;
};
