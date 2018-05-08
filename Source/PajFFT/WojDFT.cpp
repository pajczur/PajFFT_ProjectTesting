/*
 ==============================================================================
 
     WojDFT.cpp
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

#include "WojDFT.h"


// =========================================================================================================================================
// == C O N S T R U C T O R ==== D E S T R U C T O T =======================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
WojDFT::WojDFT()
{
    fPi = 4.0f * atan(1.0f);

    wSampleRate = 0.0f;
    wBufferSize = 0.0f;
    
    low_End = 0.0f;
    top_End = 0.0f;
    
    rememberedForwardOrBackward=true;
    isWindowing = false;
    
    imaginary_j.real(0.0f);
    imaginary_j.imag(1.0f);
    cZero.real(0.0f);
    cZero.imag(0.0f);
    fZero = 0.0f;
    
    phaseRotation = 1.0f;
    wPhase = 0.0;
}


WojDFT::~WojDFT()
{
    
}






// =========================================================================================================================================
// == S E T T I N G S ======================================================================================================================
// =========================================================================================================================================

// ==== PRIVATE: ====
void WojDFT::setSampleRate                       (float sampleR)
{
    wSampleRate = sampleR;
    updateFreqRangeScale(low_End, top_End);
}


void WojDFT::setBufferSize                       (float bufferS)
{
    wBufferSize= bufferS;
    resetOutputData();
    updateFreqRangeScale(low_End, top_End);
}


void WojDFT::resetOutputData                     ()
{
    if(isComplexOutput)
    {
        wOutputDataC->resize(wBufferSize);
        for(int i=0; i<wOutputDataC->size(); i++)
        {
            wOutputDataC->at(i).real(0.0f);
            wOutputDataC->at(i).imag(0.0f);
        }
    }
    else
    {
        wOutputData->resize(wBufferSize);
        for(int i=0; i<wOutputData->size(); i++)
        {
            wOutputData->at(i) = 0.0f;
        }
    }
}


void WojDFT::updateFreqRangeScale                (float lEnd, float tEnd)
{
    setLowEnd(lEnd);
    setTopEnd(tEnd);
}


void WojDFT::set_nk_bounds                       (bool forwardTRUE_backwardFALSE)
{
    if(forwardTRUE_backwardFALSE)
    {
        kStart=wBufferSize*(low_End/wSampleRate);
        kEnd=wBufferSize*(top_End/wSampleRate);
        nStart=0;
        nEnd=wBufferSize;
    }
    else
    {
        kStart=0;
        kEnd=wBufferSize;
        nStart=0;
        nEnd=wBufferSize;
    }
}

// ==== PUBLIC: ====
void WojDFT::wSettings                           (float sampleRate, float bufferSize, std::vector<float> &wOutput, bool forwardTRUE_backwardFALSE)
{
    wOutputData = &wOutput;
    isComplexOutput = false;
    rememberedForwardOrBackward = forwardTRUE_backwardFALSE;
    
    if(top_End == 0.0f)
    {
        low_End = 0.0f;
        top_End = sampleRate/2.0f;
    }
    
    setSampleRate(sampleRate);
    setBufferSize(bufferSize);
    if(top_End==0)
        top_End=sampleRate;
    
    prepareWindowingArray();
    prepareTwiddlesArray(rememberedForwardOrBackward);
    set_nk_bounds(rememberedForwardOrBackward);
}

void WojDFT::wSettings                           (float sampleRate, float bufferSize, std::vector<std::complex<float>> &wOutputC, bool forwardTRUE_backwardFALSE)
{
    wOutputDataC = &wOutputC;
    isComplexOutput = true;
    rememberedForwardOrBackward = forwardTRUE_backwardFALSE;
    
    if(top_End == 0.0f)
    {
        low_End = 0.0f;
        top_End = sampleRate/2.0f;
    }
    
    setSampleRate(sampleRate);
    setBufferSize(bufferSize);
    if(top_End==0)
        top_End=sampleRate;
    
    prepareWindowingArray();
    prepareTwiddlesArray(rememberedForwardOrBackward);
    set_nk_bounds(rememberedForwardOrBackward);
}


void WojDFT::setLowEnd                           (float lowEnd)
{
    low_End = lowEnd;
    set_nk_bounds(rememberedForwardOrBackward);
}


void WojDFT::setTopEnd                           (float topEnd)
{
    top_End = topEnd;
    set_nk_bounds(rememberedForwardOrBackward);
}


void WojDFT::setPhase                            (float phase)
{
    wPhase = phase;
    phaseRotation = pow(imaginary_j, wPhase);
}



void WojDFT::setWindowing                        (bool wind)
{
    isWindowing = wind;
}






// =========================================================================================================================================
// == P R E == C R E A T I O N =============================================================================================================
// =========================================================================================================================================

// ==== PRIVATE: ====
void WojDFT::prepareTwiddlesArray                (bool forwardOrBackward)
{
    wnkN.resize(wBufferSize);
    if(forwardOrBackward)
    {
        if(isComplexOutput) forwBackChooser=&WojDFT::freqMagnCalc_ComplexOut;
        else                forwBackChooser=&WojDFT::freqMagnitudeCalculator;
        
        for(unsigned int k=0; k<wBufferSize; k++)
        {
            wnkN[k] = twiddleCalculator((float)k);
        }
    }
    else
    {
        forwBackChooser=&WojDFT::waveAmplitudeCalculator;
        for(unsigned int k=0; k<wBufferSize; k++)
        {
            wnkN[k] = twiddleCalculator(-(float)k);
        }
    }
}


void WojDFT::prepareWindowingArray               ()
{
    windowHann.clear();
    for(int i=0; i<wBufferSize; i++)
    {
        if(i==0  ||  i==wBufferSize-1)
        {
            windowHann.push_back(0.0f);
        }
        else
        {
            float windowSample = pow(sin(fPi*i/(wBufferSize-1.0f)), 2.0);
            windowHann.push_back(windowSample);
        }
    }
}






// =========================================================================================================================================
// == D F T == A L G O R I T H M ===========================================================================================================
// =========================================================================================================================================

// ==== PUBLIC: ====
void WojDFT::makeDFT                             (std::vector<float> inputSignal)
{
    for(int k=kStart; k < kEnd; k++)
    {
        std::complex<float> sfx  = 0.0f;
        
        for (int n=nStart; n<nEnd; ++n)
        {
            sfx += inputSignal[n]     * wnkN[(k*n)%(int)wBufferSize];
        }
        
        (this->*forwBackChooser)(sfx, k);
    }
}

void WojDFT::makeDFT                             (std::vector<std::complex<float>> inputSignalC)
{
    for(int k=kStart; k < kEnd; k++)
    {
        std::complex<float> sfx  = 0.0f;
        
        for (int n=nStart; n<nEnd; ++n)
        {
            sfx += inputSignalC[n]     * wnkN[(k*n)%(int)wBufferSize];
        }
        
        (this->*forwBackChooser)(sfx, k);
    }
}






// =========================================================================================================================================
// == C A L C U L A T O R S ================================================================================================================
// =========================================================================================================================================

// ==== PRIVATE: ====
std::complex<float> WojDFT::twiddleCalculator    (float nXk)
{
    std::complex<float> wnk_N_temp;
    if((int)(nXk) % (int)wBufferSize == 0)
    {
        wnk_N_temp.real(1.0f);
        wnk_N_temp.imag(0.0f);
    }
    else if(((int)(nXk)%((int)wBufferSize/2)==0) && ((int)(nXk) % (int)wBufferSize != 0) && ((int)wBufferSize%2 == 0))
    {
        wnk_N_temp.real(-1.0f);
        wnk_N_temp.imag(0.0f);
    }
    else
    {
        wnk_N_temp.real(cosf(-2.0f * fPi * nXk / wBufferSize));
        wnk_N_temp.imag(sinf(-2.0f * fPi * nXk / wBufferSize));
    }
    return wnk_N_temp;
}


void WojDFT::freqMagnitudeCalculator            (std::complex<float> fftOutput, int freqBin)
{
    wOutputData->at(freqBin) = pow( (fftOutput.real() * fftOutput.real()) + (fftOutput.imag() * fftOutput.imag()), 0.5f ) / (wBufferSize/2.0f);
}


void WojDFT::freqMagnCalc_ComplexOut             (std::complex<float> fftOutput, int freqBin)
{
        wOutputDataC->at(freqBin) = fftOutput;
}


void WojDFT::waveAmplitudeCalculator            (std::complex<float> fftOutput, int index)
{
    fftOutput *= phaseRotation;
    float window;
    
    if(isWindowing)
        window = windowHann[index];
    else
        window = 1.0;
    
    wOutputData->at(index) = (fftOutput.real() *window)/wBufferSize;
}






// =========================================================================================================================================
// == G E T == I N F O R M A T I O N S =====================================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
float WojDFT::getBufferSize()
{
    return wBufferSize;
}


float WojDFT::getLowEnd()
{
    return low_End;
}


float WojDFT::getTopEnd()
{
    return top_End;
}


float WojDFT::getPhase()
{
    return wPhase;
}
