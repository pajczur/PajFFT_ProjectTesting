/*
  ==============================================================================

    WojFFTm.h
    Created: 2 Apr 2018 1:19:07pm
    Author:  Wojtek Pilwinski

  ==============================================================================
  ==============================================================================
  ==================== MIXED RADIX (MATRIX) FFT ALGORITHM ======================
  ==============================================================================
  ==============================================================================
 
     DESCRIPTION:
 
     You can:
     1) Perform forward or backward (inverse) Mixed Radix FFT - works for most buffer sizes
 
     2) Decompose matrix by changing divider to fit best with your buffer size
 
     3) Print matrix properties to console to have it under control
 
  ==============================================================================
 
     MANUAL
     1) Declare variable of type "WojFFTm"
 
     2) Call "wSettings(float sampleRate, float bufferSize);" to prepare all necessary data. By default it prepares data to perform forward FFT.
        To choose forward or invers DFT data preparation, call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE);"
 
     3) Now everything is ready to perform FFT by calling "makeFFT(std::vector<float> inputSignal);"
 
     4) To get output data just use "outputData" which is std::vector of type float, and it's public.
 
     OPTIONAL:
     5) You can decompose matrix by changing divider:
        To make it just call "wSetRadixDivider(int divider);"
        COMMENT:
        By default devider is 4, which I found is optimal for most buffer sizes.
        But sometimes it works better for devider = 3 or 5 or other values. It depends on buffer size.
 
     6) You can check properties of prepared matrix by calling "printRadixDimensionsInTheConsole();"
        It will print all dimensions and size of the matrix.
 
  ==============================================================================
 
     BUGS TO IMPROVE
     1) Thera are problems with amplitude of inverse DFT output.
        For buffer size equal to sample rate everything is OK,
        but for buffer size < sample rate thera are unproper results.
 
     2) Implement changing Forward/Backward after settings.
        Now you can set it only by call "wSettings(float sampleRate, float bufferSize, bool forwardTRUE_backwardFALSE)"
 
  ==============================================================================
 */

#pragma once
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>

class PajFFT_MixedRadix
{
    // ===========================================================================================================================================
    // == FUNCTIONS ==============================================================================================================================
    // ===========================================================================================================================================
public:
    PajFFT_MixedRadix();
    ~PajFFT_MixedRadix();
    void printRadixDimensionsInTheConsole(); // Just to see size and dimensions of radix - check if it's as expected
    
    // == SETTINGS ====================================================
private:
    void setSampleRate                       (float sampleR);
    void setBufferSize                       (float bufferS);
    void resetData                           (bool  forwBack);
    void resetOutput                         ();
    void updateFreqRangeScale                (float lEnd, float tEnd);
public:
    void wSettings                           (float sampleRate, float bufferSize,  bool forwardTRUE_backwardFALSE);
    void setOutputAddress                    (std::vector<             float>  &wOutput );
    void setOutputAddress                    (std::vector<std::complex<float>> &wOutputC);
    void wSetRadixDivider                    (int   divider);
    void setLowEnd                           (float lowEnd);
    void setTopEnd                           (float topEnd);
    void setPhase                            (float phase);
    void setWindowing                        (bool wind);
    
    

    // == PRE CREATION ================================================
private:
    void prepareMatrix                       (int  divider);
    void mixedRadixInputAndTwiddleComponents (int  radixStep);
    void prepareIteratorsBounds              (int  radixIterationEnd);
    void prepareTwiddlesArray                (bool forwardOrBackward);
    void prepareWindowingArray               ();
    
    
    
    // == FFT ALGORITHM ===============================================
private:
    void dftRecursion                        (int fftss, int radStep);
    void makeDFT                             (int wFFT);
public:
    void makeFFT                             (std::vector<             float>  inputSignal);
    void makeFFT                             (std::vector<std::complex<float>> inputSignal);
    void makeFFT                             (std::vector<std::complex<float>> inputSignal, std::vector<std::complex<float>> &wOutputC);
    void makeFFT                             (std::vector<             float>  inputSignal, std::vector<std::complex<float>> &wOutputC);
    
    
    
    // == CALCULATORS =================================================
private:
    std::complex<float>         twiddleCalculator        (float nXk);
public:
    void                        freqMagnitudeCalculator  (std::complex<float> fftOutput, int freqBin);
    float                       freqMagnitudeCalc        (std::complex<float> fftOutput, int freqBin);
    void                        freqMagnCalc_ComplexOut  (std::complex<float> fftOutput, int freqBin);
    void                        waveAmplitudeCalculator  (std::complex<float> fftOutput, int index);
    float                       waveEnvelopeCalc         (std::complex<float> fftOutput, int index);
    float                       phaseCalculator          (std::complex<float> fftOutput, int index);
    void   (PajFFT_MixedRadix::*forwBackChooser)         (std::complex<float> fftOutput, int freqBinOrIndex);
  
    
    
    // == GET INFORMATIONS ==========================================================
public:
    float              getBufferSize   ();
    float              getSampleRate   ();
    int                getRadDivider   ();
    std::vector<float> getRadDimensions();
    bool               isForward       ();
    float              getLowEnd       ();
    float              getTopEnd       ();
    float              getPhase        ();
    
    
    // ===========================================================================================================================================
    // == VARIABLES ==============================================================================================================================
    // ===========================================================================================================================================
private:
    double fPi;
    std::complex<float> imaginary_j;
    std::complex<float> cZero;
    float fZero;
    std::complex<float> phaseRotation;
    
    float wSampleRate;
    float wBufferSize;
    float low_End;
    float top_End;
    float lEndScale;
    float tEndScale;
    double wPhase;
    
    
    std::vector<float>                       wMixedRadix;             // Array of radix (matrix) dimensions
    int                                      wRadixSize;              // Radix size (how many dimensional matrix)
    int                                      tempRadixDimension;      // Temporarily radix dimension holder
    std::vector<float>                       xNi;                     // Array of variables which are used to prepare array of input indexes
    std::vector<float>                       xNt;                     // Array of variables which are used to prepare array of twiddle indexes
    std::vector<float>                       xNo;                     // Array of variables which are used to prepare array of twiddle indexes
    std::vector<std::complex<float>>         wnkN_forw;               // Array of precalculated forward twiddle
    std::vector<std::complex<float>>         wnkN_back;               // Array of precalculated backward twiddle
    std::vector<int*>                        iteratorsPointer;        // Array of pointers to each radix iterator needed for recursion
    std::vector<int>                         iteratorsStore;          // Array of each radix iterator
    std::vector<std::vector<int>>            iteratorsEnd;            // Array of each radix iterator bound/end
    std::vector<std::complex<float>>         sN0;                     // Temporarily input complex of each radix DFT - even
    std::vector<std::complex<float>>         sN1;                     // Temporarily input complex of each radix DFT - odd
    std::vector<float>                       windowHann;
    

    std::vector<std::vector<std::vector<int>>>              nIndex;   // Array of decomposed input indexes
    std::vector<std::vector<std::vector<std::vector<int>>>> kIndex;   // Array of decomposed output indexes
    
    
    int  radixDivider;
    bool dataPreparedConfirm;
    bool sampleRateConfirm;
    bool bufferSizeConfirm;
    bool rememberedForwardOrBackward;
    bool isWindowing;
    
//    bool isComplexOutput;
public:
    std::vector<             float>  *wOutputData;
    std::vector<std::complex<float>> *wOutputDataC;
};
