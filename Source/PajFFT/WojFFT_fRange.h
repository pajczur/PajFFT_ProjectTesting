/*
  ==============================================================================

    WojFFT_d.h
    Created: 29 Mar 2018 6:40:48pm
    Author:  Wojtek Pilwinski

 ==============================================================================
 ==============================================================================
 =========================== RADIX-2 FFT ALGORITHM ============================
 ======================= WITH FREQENCIES RANGE CHOOSER ========================
 ==============================================================================
 
     DESCRIPTION:
 
     You can:
     1) Perform forward or backward (inverse) Radix-2 FFT - works best on buffer sizes = 2^L
        COMMENT:
        In the depth it works only wit buffer sizes = 2^L, but for other sizes there is autonatic convertion to size 2^L
        and then zero padding happens or smaller buffer size than actuall.
 
     2) Change frequency range to calculate. (MAIN DIFFERENCE TO REGULAR RADIX-2 FFT)
        COMMENT:
        It provides FFT calculations only for choosen frequences range, so it provides faster calculations for smaller ranges
        (in oposit to regular radix-2 fft where decreasing freq range doesn't accelerate fft computing)
        Unfortunately to make it possible it is necessery to pre calculate and decompose indexes of input signal,
        and those pre calculations are very ineffective and slow proccess. So changing freq range dinamically or in place is not good idea.
        Maybe in the future I optimize index precalculations to make it more useable.
 
     3) Zero overloading for buffer sizes that are not 2^L
        a) if you turn off zero padding then FFT algorithm uses smaller sample rate than actuall sample rate,
           so output result is less accurate.
        b) Zero overloading works good only on forward FFT,
           For inverse it's turnd off automatically - for consequences please read point a)
 
 ==============================================================================
 
     MANUAL
     1) Declare variable of type "WojFFT"
 
     2) Call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE);" to prepare all necessary data.
 
     3) Now everything is ready to perform FFT by calling "makeFFT(std::vector<float> inputSignal);"
 
     4) To get output data just use "wOutputData" which is std::vector of type float, and it's public.
 
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
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>

class WojFFT_fRange
{
    
    // ===========================================================================================================================================
    // == FUNCTIONS ==============================================================================================================================
    // ===========================================================================================================================================
public:
    WojFFT_fRange();
    ~WojFFT_fRange();

    
    
    // == SETTINGS ====================================================
private:
    void setSampleRate                              (float sampleR);
    void setBufferSize                              (float bufferS);
    void resetOutputData                            ();
public:
    void wSettings                                  (float sampleRate, float bufferSize, std::vector<             float>  &wOutput,  bool forwardTRUE_backwardFALSE);
    void wSettings                                  (float sampleRate, float bufferSize, std::vector<std::complex<float>> &wOutputC, bool forwardTRUE_backwardFALSE);
    void setLowEnd                                  (float lowEnd);
    void setTopEnd                                  (float topEnd);
    void setFreqRange                               (float lowE, float topE);
    void setZeroPadding                             (bool  trueON_falseOFF);
    void setOutput                                  (std::vector<             float>  &wOutput);
    void setOutput                                  (std::vector<std::complex<float>> &wOutputC);

    
    
    // == PRE CREATION ================================================
private:
    void bitReversal                                (float bufS);
    void prepareTwiddlesArray                       (int wnkN_Size, bool forwardOrBackward);
    void prepareTwiddlesIndexesMatrix               ();
    void prepare_sN0_matrix                         ();
    void freqRangeReindex                           (float lowE, float topE);
    
    
    // == FFT ALGORITHM ===============================================
public:
    void makeFFT                                    (std::vector<             float>  inputSignal );
    void makeFFT                                    (std::vector<std::complex<float>> inputSignalC);
 
    
    // == CALCULATORS =================================================
private:
    std::complex<float>    twiddleCalculator        (float nXk);
    float                  freqMagnitudeCalculator  (std::complex<float> fftOutput);
    float                  waveAmplitudeCalculator  (std::complex<float> fftOutput);
    float (WojFFT_fRange::*forwBackChooser)         (std::complex<float>);
    
    
    // == GET INFORMATIONS ==========================================================
public:
    float getBufferSize();
    float getLowEnd    ();
    float getTopEnd    ();
    
    
    
    // ===========================================================================================================================================
    // == VARIABLES ==============================================================================================================================
    // ===========================================================================================================================================
private:
    float fPi;
    float wBufferSize;
    float wSampleRate;
    float low_End;
    float top_End;
    int   trueBuffersize;
    bool  zeroPadding;
    bool  resizeInput;
    bool  rememberedForwardOrBackward;
    bool  dataPreparedConfirm;
    
    std::vector<std::complex<float>> wnkN_forw;
    std::vector<std::complex<float>> wnkN_back;
    std::vector<int> bitReversed;
    int wModulo;
    
    std::vector<std::vector<std::complex<float>>> sN0;
    
    std::vector<std::vector<int>> wk;
    std::vector<int> freqRange;
    std::vector<std::vector<int>> wFreqDec;

    bool isComplexOutput;
public:
    std::vector<             float>  *wOutputData;
    std::vector<std::complex<float>> *wOutputDataC;
};
