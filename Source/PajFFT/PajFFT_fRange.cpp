/*
  ==============================================================================

    WojFFT_d.cpp
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
    And then zero padding happens or smaller buffer size than actuall.
 
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

#include "PajFFT_fRange.h"

// =========================================================================================================================================
// == C O N S T R U C T O R ==== D E S T R U C T O T =======================================================================================
// =========================================================================================================================================
PajFFT_fRange::PajFFT_fRange()
{
    fPi = 4.0f * atan(1.0f);
    
    wSampleRate = 0.0f;
    wBufferSize = 0.0f;
    
    low_End = 0.0f;
    top_End = 0.0f;
    
    zeroPadding = true;
    dataPreparedConfirm = false;
    rememberedForwardOrBackward=true;
}

PajFFT_fRange::~PajFFT_fRange()
{
    
}







// =========================================================================================================================================
// == S E T T I N G S ======================================================================================================================
// =========================================================================================================================================
// PRIVATE:
void PajFFT_fRange::setSampleRate                        (float sampleR)
{
    wSampleRate = sampleR;
}


void PajFFT_fRange::setBufferSize                        (float bufferS)
{
    trueBuffersize = bufferS;
    int correction = log2(bufferS);
    wBufferSize= pow(2, correction);
    
    if(wBufferSize < bufferS && zeroPadding && rememberedForwardOrBackward)
    {
        wBufferSize *= 2.0f;
    }
    
    bitReversal(wBufferSize);
    wModulo = wBufferSize/2;
    
    prepare_sN0_matrix();

    freqRangeReindex(low_End, top_End);
    
    prepareTwiddlesIndexesMatrix();
}


void PajFFT_fRange::resetOutputData                      ()
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


// PUBLIC:
void PajFFT_fRange::wSettings                            (float sampleRate, float bufferSize, std::vector<float> &wOutput, bool forwardTRUE_backwardFALSE)
{
    wOutputData = &wOutput;
    isComplexOutput = false;
    rememberedForwardOrBackward = forwardTRUE_backwardFALSE;
    
    if(top_End==0 && forwardTRUE_backwardFALSE)
        top_End=sampleRate/2.0f;
    
    if(!forwardTRUE_backwardFALSE)
    {
        low_End = 0.0f;
        top_End = wSampleRate;
    }
    
    setSampleRate(sampleRate);
    setBufferSize(bufferSize);
    resetOutputData();
    
    prepareTwiddlesArray(wBufferSize/2, forwardTRUE_backwardFALSE);
    

    if(!dataPreparedConfirm)
        dataPreparedConfirm = true;
}


void PajFFT_fRange::wSettings                            (float sampleRate, float bufferSize, std::vector<std::complex<float>> &wOutputC, bool forwardTRUE_backwardFALSE)
{
    wOutputDataC = &wOutputC;
    isComplexOutput = true;
    rememberedForwardOrBackward = forwardTRUE_backwardFALSE;
    
    if(top_End==0 && forwardTRUE_backwardFALSE)
        top_End=sampleRate/2.0f;
    
    if(!forwardTRUE_backwardFALSE)
    {
        low_End = 0.0f;
        top_End = wSampleRate;
    }
    
    setSampleRate(sampleRate);
    setBufferSize(bufferSize);
    resetOutputData();
    
    prepareTwiddlesArray(wBufferSize/2, forwardTRUE_backwardFALSE);
    
    
    if(!dataPreparedConfirm)
        dataPreparedConfirm = true;
}


void PajFFT_fRange::setLowEnd                            (float lowEnd)
{
    low_End = lowEnd;
    freqRangeReindex(low_End, top_End);
}


void PajFFT_fRange::setTopEnd                            (float topEnd)
{
    top_End = topEnd;
    freqRangeReindex(low_End, top_End);
}


void PajFFT_fRange::setFreqRange                         (float lowE, float topE)
{
    low_End = lowE;
    top_End = topE;
    freqRangeReindex(low_End, top_End);
}


void PajFFT_fRange::setZeroPadding                       (bool  trueON_falseOFF)
{
    if(rememberedForwardOrBackward)
    {
        zeroPadding = trueON_falseOFF;
        if(dataPreparedConfirm)
            wSettings(wSampleRate, trueBuffersize, *wOutputData, rememberedForwardOrBackward);
    }
    else
        return;
}


void PajFFT_fRange::setOutput                            (std::vector<             float>  &wOutput)
{
    wOutputData = &wOutput;
    isComplexOutput = false;
    resetOutputData();
}


void PajFFT_fRange::setOutput                            (std::vector<std::complex<float>> &wOutputC)
{
    wOutputDataC = &wOutputC;
    isComplexOutput = true;
    resetOutputData();
}







// =========================================================================================================================================
// == P R E == C R E A T I O N =============================================================================================================
// =========================================================================================================================================
// PRIVATE:
void PajFFT_fRange::bitReversal                          (float bufS)
{
    bitReversed.resize(bufS);
    
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


void PajFFT_fRange::prepareTwiddlesArray                 (int wnkN_Size, bool forwardOrBackward)
{
    wnkN_forw.resize(wnkN_Size);
    if(forwardOrBackward)
    {
        forwBackChooser=&PajFFT_fRange::freqMagnitudeCalculator;
        for(unsigned int i=0; i<wnkN_forw.size(); i++)
        {
            wnkN_forw[i] = twiddleCalculator((float)i);
        }
    }
    else
    {
        forwBackChooser=&PajFFT_fRange::waveAmplitudeCalculator;
        for(unsigned int i=0; i<wnkN_forw.size(); i++)
        {
            wnkN_forw[i] = twiddleCalculator(-(float)i);
        }
    }
    
    rememberedForwardOrBackward = forwardOrBackward;
}


void PajFFT_fRange::prepare_sN0_matrix                   ()
{
    sN0.resize(log2(wBufferSize)+1);
    
    for(int i=0; i<sN0.size(); i++)
    {
        std::vector<std::complex<float>> temp;
        temp.resize(wBufferSize);
        sN0[i] = temp;
    }
}


void PajFFT_fRange::freqRangeReindex                     (float lowE, float topE)
{
    freqRange.clear();
    wFreqDec.clear();
    for(int i=lowE; i<topE; i++)
    {
        freqRange.push_back(std::round(i/std::round(wSampleRate/wBufferSize)));
    }


    wFreqDec.resize(log2(wBufferSize));

    std::vector<std::vector<int>> temp;
    temp.resize(freqRange.size());
    std::vector<int> temp2;
    temp2.clear();

    for(int rdx2=0; rdx2<wFreqDec.size(); rdx2++)
    {
        for(int i=0; i<freqRange.size(); i++)
        {
            temp[i].resize(wBufferSize/pow(2, rdx2+1));
            temp[i][0] = (int)(freqRange[i]*wBufferSize/pow(2, rdx2+1))%(int)wBufferSize;

            for(int j=1; j<temp[i].size(); j++)
                temp[i][j] = temp[i][j-1] + 1;


            if(temp2.empty())
                temp2.insert(temp2.begin(), temp[i].begin(), temp[i].end());

            if(i>0)
            {
                for(int k=1; k<i+1; k++)
                {
                    if(temp[i] == temp[i-k])
                    {
                        temp[i].clear();
                        break;
                    }
                }
                if(!temp[i].empty())
                {
                    temp2.insert(temp2.end(), temp[i].begin(),temp[i].end());
                }
            }
        }

        std::sort(temp2.begin(), temp2.end());

        wFreqDec[rdx2] = temp2;
        temp2.clear();
    }
}


void PajFFT_fRange::prepareTwiddlesIndexesMatrix         ()
{
    std::vector<int> mTemp;
    std::vector<int> pTemp;
    mTemp.resize(log2(wBufferSize));
    pTemp.resize(log2(wBufferSize));

    for(int z=0; z<log2(wBufferSize); z++)
    {
        mTemp[z] = wBufferSize/pow(2, z+1);
        pTemp[z] = pow(2, z);
    }

    wk.clear();
    wk.resize(log2(wBufferSize));

    for(int fft=0; fft<log2(wBufferSize); fft++)
    {
        wk[fft].clear();
        wk[fft].resize(wBufferSize);

        for(int d=0; d<wBufferSize; d++)
            wk[fft][d] = mTemp[fft] * ((d / mTemp[fft]) % pTemp[fft]);
    }
}





// =========================================================================================================================================
// == F F T == A L G O R I T H M ===========================================================================================================
// =========================================================================================================================================
// PUBLIC:
void PajFFT_fRange::makeFFT                              (std::vector<float> inputSignal)
{
    std::vector<float> wBuffer = inputSignal;
    if(resizeInput) wBuffer.push_back(0.0f);

    for(int sample=0; sample<wBufferSize; sample++)
    {
        sN0[0][sample] = wBuffer[bitReversed[sample]];
    }

    for(int fft=0; fft<log2(wBufferSize); fft++)
    {
        for(int d=0; d<wFreqDec[fft].size(); d++)
        {
            sN0[fft+1][wFreqDec[fft][d]] = sN0[fft][2*(wFreqDec[fft][d]%wModulo)] + sN0[fft][(2*(wFreqDec[fft][d]%wModulo))+1] * wnkN_forw[wk[fft][wFreqDec[fft][d]]] * pow(-1.0f, (float)(wFreqDec[fft][d]/wModulo));

            if(fft == log2(wBufferSize) - 1)
            {
                if(isComplexOutput) wOutputDataC->at(wFreqDec[fft][d]) = sN0[fft+1][wFreqDec[fft][d]];
                else                wOutputData->at(wFreqDec[fft][d]) = (this->*forwBackChooser)(sN0[fft+1][wFreqDec[fft][d]]);
            }
        }
    }
}


void PajFFT_fRange::makeFFT                              (std::vector<std::complex<float>> inputSignalC)
{
    std::vector<std::complex<float>> wBuffer = inputSignalC;
    if(resizeInput) wBuffer.push_back(0.0f);
    
    for(int sample=0; sample<wBufferSize; sample++)
    {
        sN0[0][sample] = wBuffer[bitReversed[sample]];
    }
    
    for(int fft=0; fft<log2(wBufferSize); fft++)
    {
        for(int d=0; d<wFreqDec[fft].size(); d++)
        {
            sN0[fft+1][wFreqDec[fft][d]] = sN0[fft][2*(wFreqDec[fft][d]%wModulo)] + sN0[fft][(2*(wFreqDec[fft][d]%wModulo))+1] * wnkN_forw[wk[fft][wFreqDec[fft][d]]] * pow(-1.0f, (float)(wFreqDec[fft][d]/wModulo));
            
            if(fft == log2(wBufferSize) - 1)
            {
                if(isComplexOutput) wOutputDataC->at(wFreqDec[fft][d]) = sN0[fft+1][wFreqDec[fft][d]];
                else                wOutputData->at(wFreqDec[fft][d]) = (this->*forwBackChooser)(sN0[fft+1][wFreqDec[fft][d]]);
            }
        }
    }
}







// =========================================================================================================================================
// == C A L C U L A T O R S ================================================================================================================
// =========================================================================================================================================
// PRIVATE:
std::complex<float> PajFFT_fRange::twiddleCalculator     (float nXk)
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


float PajFFT_fRange::freqMagnitudeCalculator             (std::complex<float> fftOutput)
{
    float _Re_2;
    float _Im_2;
    _Re_2 = fftOutput.real() * fftOutput.real();
    _Im_2 = fftOutput.imag() * fftOutput.imag();
    
    //    return 20*log10(pow(_Re_2 + _Im_2, 0.5f));
    return pow(_Re_2 + _Im_2, 0.5f)/(wBufferSize/2.0f);
}


float PajFFT_fRange::waveAmplitudeCalculator             (std::complex<float> fftOutput)
{
    return fftOutput.real()/wBufferSize;
}







// =========================================================================================================================================
// == G E T == I N F O R M A T I O N S =====================================================================================================
// =========================================================================================================================================
// ==== PUBLIC: ====
float PajFFT_fRange::getBufferSize()
{
    return wBufferSize;
}

float PajFFT_fRange::getLowEnd    ()
{
    return low_End;
}

float PajFFT_fRange::getTopEnd    ()
{
    return top_End;
}
