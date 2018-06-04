/*
  ==============================================================================

     WojFFT.cpp
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
        In the depth it works only wit buffer sizes = 2^L, but for other sizes there is autonatic convertion to size 2^L
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

#include "PajFFT_Radix2.h"


// =========================================================================================================================================
// == C O N S T R U C T O R ==== D E S T R U C T O T =======================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
PajFFT_Radix2::PajFFT_Radix2()
{
    fPi = 4.0f * atan(1.0f);

    wSampleRate = 0.0f;
    wBufferSize = 0.0f;
    
    zeroPadding = true;
    bufferSizeConfirm = false;
    sampleRateConfirm = false;
    dataPreparedConfirm = false;
    rememberedForwardOrBackward=true;
    isWindowing = false;
    
    imaginary_j.real(0.0f);
    imaginary_j.imag(1.0f);
    cZero.real(0.0f);
    cZero.imag(0.0f);
    fZero = 0.0f;
    
    phaseRotation = 1.0f;
    low_End = 0.0f;
    top_End = 0.0f;
    
    wPhase = 0.0;
}


PajFFT_Radix2::~PajFFT_Radix2()
{
    
}






// =========================================================================================================================================
// == S E T T I N G S ======================================================================================================================
// =========================================================================================================================================
// ==== PRIVATE: ====
void PajFFT_Radix2::setSampleRate                          (float sampleR)
{
    wSampleRate = sampleR;
    sampleRateConfirm = true;
}


void PajFFT_Radix2::setBufferSize                          (float bufferS)
{
    trueBuffersize = bufferS;

    int correction = log2(bufferS);
    wBufferSize= pow(2, correction);
    
    if(wBufferSize < bufferS && zeroPadding/* && rememberedForwardOrBackward*/)
    {
        wBufferSize *= 2.0f;
    }
    
    wBufNyquist = wBufferSize/2.0f;
    
    bufferSizeConfirm = true;
}


void PajFFT_Radix2::resetData                        ()
{
//    if(isComplexOutput)
//    {
//        wOutputDataC->resize(wBufferSize);
//        for(int i=0; i<wOutputDataC->size(); i++)
//        {
//            wOutputDataC->at(i).real(0.0f);
//            wOutputDataC->at(i).imag(0.0f);
//        }
//    }
//    else
//    {
//        wOutputData->resize(wBufferSize);
//        for(int i=0; i<wOutputData->size(); i++)
//        {
//            wOutputData->at(i) = 0.0f;
//        }
//    }
    if(sampleRateConfirm && bufferSizeConfirm)
    {
        bitReversal(wBufferSize);
        prepare_sN0_matrix();
        prepareWindowingArray();
        updateFreqRangeScale(low_End, top_End);
        prepareTwiddlesArray();
    }
}


void PajFFT_Radix2::updateFreqRangeScale   (float lEnd, float tEnd)
{
    setLowEnd(lEnd);
    setTopEnd(tEnd);
}

// ==== PUBLIC: ====
void PajFFT_Radix2::wSettings                              (float sampleRate, float bufferSize)
{
    if(top_End == 0.0f)
    {
        low_End = 0.0f;
        top_End = sampleRate/2.0f;
    }
    
    setSampleRate(sampleRate);
    setBufferSize(bufferSize);
    
    resetData();

    if(!dataPreparedConfirm)
    dataPreparedConfirm = true;
}


void PajFFT_Radix2::setLowEnd                              (float lowEnd)
{
    low_End = lowEnd;
    lEndScale = wBufferSize * (lowEnd/wSampleRate);
}


void PajFFT_Radix2::setTopEnd                              (float topEnd)
{
    top_End = topEnd;
    tEndScale = wBufferSize * (topEnd/wSampleRate);
}


void PajFFT_Radix2::setZeroPadding                         (bool  trueON_falseOFF)
{
    zeroPadding = trueON_falseOFF;
    if(dataPreparedConfirm)
        wSettings(wSampleRate, trueBuffersize);
}


void PajFFT_Radix2::setPhase                               (float phase)
{
    wPhase = phase;
    phaseRotation = pow(imaginary_j, wPhase);
}


void PajFFT_Radix2::setWindowing                           (bool wind)
{
    isWindowing = wind;
}






// =========================================================================================================================================
// == P R E == C R E A T I O N =============================================================================================================
// =========================================================================================================================================
// ==== PRIVATE: ====
void PajFFT_Radix2::bitReversal                            (float bufSize)
{
    bitReversed.resize(bufSize);
    
    for(int i=0; i<bitReversed.size(); i++)
    {
        bitReversed[i] = i;
    }
    
    unsigned long nn, n, m, j;
    
    nn = bitReversed.size()/2;
    n  = nn<<1;
    j  = 1;
    
    for (int i=1; i<n; i+=1)
    {
        if (j>i)
            std::swap(bitReversed[j-1], bitReversed[i-1]);
        
        m = nn;
        while (m>=2 && j>m)
        {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    
    if(zeroPadding && bitReversed.size() > trueBuffersize)
    {
        resizeInput = true;
        for(int i=0; i<bitReversed.size(); i++)
        {
            if(bitReversed[i] > trueBuffersize)
                bitReversed[i] = trueBuffersize;
        }
    }
    else
    {
        resizeInput = false;
    }
}


void PajFFT_Radix2::prepareTwiddlesArray                   ()
{
    wnkN_forw.resize(wBufferSize);
    wnkN_back.resize(wBufferSize);
    
    for(unsigned int i=0; i<wBufferSize; i++)
    {
        wnkN_forw[i] = twiddleCalculator((float)i);
        wnkN_back[i] = 1.0f/(wnkN_forw[i]);
    }
}


void PajFFT_Radix2::prepare_sN0_matrix                     ()
{
    sN0.resize(log2(wBufferSize));
    for(int z=0; z<sN0.size(); z++)
    {
        sN0[z].resize(wBufferSize/pow(2, z+1));
        
        for(int i=0; i<sN0[z].size(); i++)
        {
            std::vector<std::complex<float>> temp;
            temp.resize(pow(2, z+1));
            sN0[z][i] = temp;
        }
    }
}


void PajFFT_Radix2::prepareWindowingArray                  ()
{
    windowHann.clear();
    for(int i=0; i<wBufferSize; ++i)
    {
        if(i==0  ||  i==wBufferSize-1)
        {
            windowHann.push_back(0.0f);
        }
        else
        {
            float windowSample = -0.5*cos(2.*fPi*(double)i/(double)wBufferSize)+0.5;
            windowHann.push_back(windowSample);
        }
    }
}




// =========================================================================================================================================
// == F F T == A L G O R I T H M ===========================================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
void PajFFT_Radix2::makeFFT                                (std::vector<float> inputSignal, std::vector<std::complex<float>> &wOutputC, bool isForwardOrNot)
{
    std::vector<std::complex<float>>& wnkNXX = isForwardOrNot?wnkN_forw:wnkN_back;
    isForward=isForwardOrNot;
    wOutputData = &wOutputC;
//    wOutputData->resize(wBufferSize);
    std::vector<float> wBuffer = inputSignal;
    if(resizeInput) wBuffer.push_back(0.0f);
//    if(resizeInput) inputSignal.push_back(0.0f);

    for(int radix2=0; radix2<sN0.size(); radix2++)
    {
        if      (radix2==0)
            firstStepFFT(wBuffer, radix2);

        else if (radix2 >0 && radix2<sN0.size()-1)
            divideAndConquereFFT(radix2, wnkNXX);

        else
            lastStepFFT(radix2, wnkNXX);
    }
}

void PajFFT_Radix2::makeFFT                                (std::vector<std::complex<float>> inputSignalC, std::vector<std::complex<float>> &wOutputC, bool isForwardOrNot)
{
    std::vector<std::complex<float>>& wnkNXX = isForwardOrNot?wnkN_forw:wnkN_back;
    isForward=isForwardOrNot;
    wOutputData = &wOutputC;
//    wOutputData->resize(wBufferSize);
    std::vector<std::complex<float>> wBuffer = inputSignalC;
    if(resizeInput) wBuffer.push_back(0.0f);
//    if(resizeInput) inputSignalC.push_back(0.0f);

    
    for(int radix2=0; radix2<sN0.size(); radix2++)
    {
        if      (radix2==0)
            firstStepFFTc(wBuffer, radix2);
        
        else if (radix2 >0 && radix2<sN0.size()-1)
            divideAndConquereFFT(radix2, wnkNXX);
        
        else
            lastStepFFT(radix2, wnkNXX);
    }
}


// PRIVATE:
void PajFFT_Radix2::firstStepFFT                           (std::vector<float> inputSignal, int rdx2)
{
    for(int k=0; k<wBufferSize/pow(2, rdx2+1); k++)
    {
        for(int n=0; n<pow(2, rdx2+1); n++)
        {
            sN0[rdx2][k][n]   = inputSignal[bitReversed[2*k]]
                              + pow(-1.0f, (float)(n/1))
                              * inputSignal[bitReversed[2*k+1]];
        }
    }
}

void PajFFT_Radix2::firstStepFFTc                          (std::vector<std::complex<float>> inputSignalc, int rdx2)
{
    for(int k=0; k<wBufferSize/pow(2, rdx2+1); k++)
    {
        for(int n=0; n<pow(2, rdx2+1); n++)
        {
            sN0[rdx2][k][n]   = inputSignalc[bitReversed[2*k]]
            + pow(-1.0f, (float)(n/1))
            * inputSignalc[bitReversed[2*k+1]];
        }
    }
}


void PajFFT_Radix2::divideAndConquereFFT                   (int rdx2, std::vector<std::complex<float>> &twiddle)
{
    for(int k=0; k<wBufferSize/pow(2, rdx2+1); k++)
    {
        for(int n=0; n<pow(2, rdx2+1); n++)
        {
            sN0[rdx2][k][n] = sN0[rdx2-1] [2*k][n%(int)pow(2, rdx2)]
                            +
                            pow(-1.0f, (float)(n/(int)pow(2, rdx2)))
                            *
                            (
                                  sN0[rdx2-1][2*k+1][n%(int)pow(2, rdx2)]
//                                * wnkN[(n%(int)pow(2, rdx2)) * (int)(wBufferSize/pow(2.0f, (float)rdx2+1.0f))]
                                * twiddle[(n%(int)pow(2, rdx2)) * (int)(wBufferSize/pow(2.0f, (float)rdx2+1.0f))]
                            );
        }
    }
}


void PajFFT_Radix2::lastStepFFT                            (int rdx2, std::vector<std::complex<float>> &twiddle)
{
    for(int k=0; k<wBufferSize/pow(2, rdx2+1); k++)
    {
        for(int n=0; n<wBufferSize; n++)
        {
            sN0[rdx2][k][n] = sN0[rdx2-1] [2*k][n%(int)pow(2, rdx2)]
                            +
                            pow(-1.0f, (float)(n/(int)pow(2, rdx2)))
                            *
                            (
                                 sN0[rdx2-1][2*k+1][n%(int)pow(2, rdx2)]
//                               * wnkN[(n%(int)pow(2, rdx2)) * (int)(wBufferSize/pow(2.0f, (float)rdx2+1.0f))]
                               * twiddle[(n%(int)pow(2, rdx2)) * (int)(wBufferSize/pow(2.0f, (float)rdx2+1.0f))]
                            );
            
            if(isForward   &&   n>=wBufNyquist)
                wOutputData->at(n) = 0.0f;
            else
                wOutputData->at(n) = sN0[rdx2][k][n];
//            (this->*forwBackChooser)(sN0[rdx2][k][n], n);
            
        }
    }
}






// =========================================================================================================================================
// == C A L C U L A T O R S ================================================================================================================
// =========================================================================================================================================
// ==== PRIVATE: ====
std::complex<float> PajFFT_Radix2::twiddleCalculator       (float nXk)
{
    std::complex<float> wnk_N_temp;
    if((int)nXk % (int)wBufferSize == 0)
    {
        wnk_N_temp.real(1.0f);
        wnk_N_temp.imag(0.0f);
    }
    else if(((int)nXk%((int)wBufferSize/2)==0) && ((int)nXk % (int)wBufferSize != 0) && ((int)wBufferSize%2 == 0))
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


float PajFFT_Radix2::freqMagnitudeCalc               (std::complex<float> fftOutput, long freqBin)
{
    if(freqBin<lEndScale  ||  freqBin>tEndScale)
        return fZero;
    else
    {
        float _Re_2;
        float _Im_2;
        _Re_2 = fftOutput.real() * fftOutput.real();
        _Im_2 = fftOutput.imag() * fftOutput.imag();

        return pow(_Re_2 + _Im_2, 0.5f);
    }
}


float PajFFT_Radix2::waveEnvelopeCalc                       (std::complex<float> fftOutput, long index)
{
    fftOutput *= phaseRotation;
    
    float window;
    
    if(isWindowing)
        window = windowHann[index];
    else
        window = 1.0;
    
    wOutputData->at(index) = (fftOutput.real()*window)/wBufferSize;
    return windowing(fftOutput.real(), index)/((long)wBufferSize);
}

float PajFFT_Radix2::phaseCalculator          (std::complex<float> fftOutput, long index)
{
    return atan2(fftOutput.imag(),fftOutput.real());
}

std::complex<float> PajFFT_Radix2::windowing(std::complex<float> dataToWindowing, long index)
{
    if(isWindowing)
        return dataToWindowing*windowHann[index];
    else
        return dataToWindowing;
}

float PajFFT_Radix2::windowing(float dataToWindowing, long index)
{
    if(isWindowing)
        return dataToWindowing*windowHann[index];
    else
        return dataToWindowing;
}


// =========================================================================================================================================
// == G E T == I N F O R M A T I O N S =====================================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
float PajFFT_Radix2::getBufferSize()
{
    return wBufferSize;
}


float PajFFT_Radix2::getLowEnd()
{
    return low_End;
}


float PajFFT_Radix2::getTopEnd()
{
    return top_End;
}


float PajFFT_Radix2::getPhase()
{
    return wPhase;
}
