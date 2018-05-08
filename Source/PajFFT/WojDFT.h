/*
 ==============================================================================
 
     WojDFT.h
     Created: 13 Mar 2018 9:11:15pm
     Author:  Wojtek Pilwinski
 
 ==============================================================================
 ==============================================================================
 =========================== REGULAR DFT ALGORITHM ============================
 ==============================================================================
 ==============================================================================
 
     DESCRIPTION
 
     You can:
     1) Perform forward or backward (inverse) DFT
 
     2) Change frequency range to calculate,
        so you can make calculations little bit faster.
        How much faster? It depends on the range you choose. Remember it's regular DFT.
 
 ==============================================================================
 
     MANUAL
     1) Declare variable of type "WojDFT"
 
     2) Call "wSettings(float sampleRate, float bufferSize);" to prepare all necessary data. By default it prepares data to perform forward DFT.
         To choose forward or invers DFT data preparation, call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE);"
 
     3) Now everything is ready to perform DFT by calling "makeDFT(std::vector<float> inputSignal);"
 
     4) To get output data just use "outputData" which is std::vector of type float, and it's public.
 
     5) You can set freq range by:
         a) setLowEnd(float lowEnd); WARNING: lowEnd can't be less than zero,   and can't be greater than topEnd.
         b) setTopEnd(float topEnd); WARNING: topEnd can't be less than lowEnd, and can't be greater than sample rate.
 
         REMEMBER:
         If you don't set freq range, by default they are from zero to Nyquist freq:
         low_End = 0;
         top_End = sampleRate/2;
 
 ==============================================================================
 
     COMMENTS
     BUGS TO IMPROVE
     1) Thera are problems with amplitude of inverse DFT output.
        For buffer size equal to sample rate everything is OK,
        but for buffer size < sample rate thera are unproper results.
 
 ==============================================================================
 */

#pragma once
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>




class WojDFT
{
    
    // ===========================================================================================================================================
    // == FUNCTIONS ==============================================================================================================================
    // ===========================================================================================================================================
public:
    WojDFT();
    ~WojDFT();
    
    
    // == SETTINGS ==================================================================
private:
    void setSampleRate                           (float sampleR);
    void setBufferSize                           (float bufferS);
    void resetOutputData                         ();
    void set_nk_bounds                           (bool forwardTRUE_backwardFALSE);
    void updateFreqRangeScale                    (float lEnd, float tEnd);
public:
    void wSettings                               (float sampleRate, float bufferSize, std::vector<             float>  &wOutput,  bool forwardTRUE_backwardFALSE);
    void wSettings                               (float sampleRate, float bufferSize, std::vector<std::complex<float>> &wOutputC, bool forwardTRUE_backwardFALSE);
    void setLowEnd                               (float lowEnd); // By default low_End = 0
    void setTopEnd                               (float topEnd); // By default top_End = sample rate
    void setPhase                                (float phase);
    void setWindowing                            (bool wind);

    
    
    // == PRE CREATION ==============================================================
private:
    void prepareTwiddlesArray                    (bool forwardOrBackward);
    void prepareWindowingArray                   ();

    
    
    // == D F T == ALGORITHM ========================================================
public:
    void makeDFT                                 (std::vector<             float>  inputSignal);
    void makeDFT                                 (std::vector<std::complex<float>> inputSignalC);
    
    

    // == CALCULATORS ===============================================================
private:
    std::complex<float> twiddleCalculator        (float nXk);
    void                freqMagnitudeCalculator  (std::complex<float> fftOutput, int freqBin);
    void                freqMagnCalc_ComplexOut  (std::complex<float> fftOutput, int freqBin);
    void                waveAmplitudeCalculator  (std::complex<float> fftOutput, int index);
    void      (WojDFT::*forwBackChooser)         (std::complex<float> fftOutput, int freqBinOrIndex);
    
    
    
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
    float wBufferSize;
    float wSampleRate;
    int   kStart;
    int   kEnd;
    int   nStart;
    int   nEnd;
    float low_End;
    float top_End;
    bool  rememberedForwardOrBackward;
    std::complex<float> phaseRotation;
    double wPhase;
    
    std::vector<std::complex<float>>  wnkN; // Array of precalculated twiddle - W^nk
    std::vector<float>                windowHann;
    
    bool isWindowing;
    bool isComplexOutput;
public:
    std::vector<             float>  *wOutputData;
    std::vector<std::complex<float>> *wOutputDataC;
    
};

