/*
  ==============================================================================

    WojFFTm.cpp
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

#include "PajFFT_MixedRadix.h"






// =========================================================================================================================================
// == C O N S T R U C T O R ==== D E S T R U C T O T =======================================================================================
// =========================================================================================================================================

PajFFT_MixedRadix::PajFFT_MixedRadix()
{
    fPi = 4.0 * atan(1.0);
    wSampleRate = 0.0f;
    wBufferSize = 0.0f;
    
    radixDivider=4; // default radix divider
    dataPreparedConfirm=false;
    sampleRateConfirm=false;
    bufferSizeConfirm=false;
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


PajFFT_MixedRadix::~PajFFT_MixedRadix()
{
    
}


void PajFFT_MixedRadix::printRadixDimensionsInTheConsole()
{
    if(!wMixedRadix.empty())
    {
        for(int i=0; i<wMixedRadix.size(); i++)
        {
            std::cout << "Matrix " << i << ": " << wMixedRadix[i] << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "MATRIX SIZE: " << wRadixSize << std::endl << std::endl;
    }
    else return;
}






// =========================================================================================================================================
// == S E T T I N G S ======================================================================================================================
// =========================================================================================================================================

// PRIVATE:
// == Set sample rate ========================================================================
// ===========================================================================================
void PajFFT_MixedRadix::setSampleRate          (float sampleR)
{
    wSampleRate = sampleR;
    sampleRateConfirm = true;
}




// == Set buffer size =========================================================================
// ============================================================================================
void PajFFT_MixedRadix::setBufferSize          (float bufferS)
{
    wBufferSize = bufferS;
    wBufNyquist = wBufferSize/2.0f;
    bufferSizeConfirm = true;

}




// == Reseting all data, prepare vectors, and pre calculate all twiddle factors ===============
// ============================================================================================
void PajFFT_MixedRadix::resetData              ()
{
    if(sampleRateConfirm && bufferSizeConfirm)
    {
        prepareWindowingArray();
        /* Prepare matrix dimensions */
        wMixedRadix.clear();
        prepareMatrix(radixDivider);
        
        /* Prepare indexes for input and twiddle components */
        nIndex.clear();
        kIndex.clear();
        mixedRadixInputAndTwiddleComponents(0);
        
        /* Colecting addresses of all radix iterators */
        iteratorsStore.resize(wRadixSize);
        iteratorsPointer.resize(wRadixSize);
        for(int i=0; i<wRadixSize; i++)
        {
            iteratorsPointer[i] = &iteratorsStore[i];
        }

        /* Prepare array of iterators ends */
        iteratorsEnd.resize(wRadixSize);
        prepareIteratorsBounds(0);

        /* Precalculations of all twiddles W = exp(-i*2*fPi*n*k / N) */
        prepareTwiddlesArray();

        dataPreparedConfirm = true;
    }
    else
    {
        return;
    }
}



// PUBLIC:
// == Settings == you can choose vector output ================================================
// ============================================================================================
void PajFFT_MixedRadix::wSettings         (float sampleRate, float bufferSize)
{
    if(top_End == 0.0f)
    {
        setLowEnd(0.0f);
        setTopEnd(sampleRate/2.0);
    }
    
    setBufferSize(bufferSize);
    setSampleRate(sampleRate);

    resetData();
}



// == Change radix divider == By default it's 4 ===============================================
// ============================================================================================
void PajFFT_MixedRadix::wSetRadixDivider  (int divider)
{
    radixDivider = divider;
    
    if(wSampleRate!=0 && wBufferSize!=0)
    wSettings(wSampleRate, wBufferSize);
}




void PajFFT_MixedRadix::setLowEnd         (float lowEnd)
{
    low_End = lowEnd;
    lEndScale = wBufferSize * (lowEnd/wSampleRate);
}




void PajFFT_MixedRadix::setTopEnd         (float topEnd)
{
    top_End = topEnd;
    tEndScale = wBufferSize * (topEnd/wSampleRate);
}



void PajFFT_MixedRadix::setPhase          (float phase)
{
    wPhase = phase;
    phaseRotation = pow(imaginary_j, wPhase);
}



void PajFFT_MixedRadix::setWindowing      (bool wind)
{
    isWindowing = wind;
}







// =========================================================================================================================================
// == P R E == C R E A T I O N =============================================================================================================
// =========================================================================================================================================

// PRIVATE:
// == It creates input indexes matrix ========================================================
// ===========================================================================================
void PajFFT_MixedRadix::prepareMatrix                        (int divider)
{
    if((wMixedRadix.empty()?(int)wBufferSize:tempRadixDimension) % divider == 0)
    {
        tempRadixDimension = (wMixedRadix.empty()?wBufferSize:tempRadixDimension)/divider;
        wMixedRadix.insert(wMixedRadix.begin(), (float)divider);
        
        divider=radixDivider;
        prepareMatrix(divider);
    }
    else
    {
        if(divider < (wMixedRadix.empty()?wBufferSize:tempRadixDimension))
        {
            int divTemp = divider+1;
            prepareMatrix(divTemp);
        }
        else
        {
            int matrixChecker = 1; // check if matrix is is full and ready to use
            for(int i=0; i<wMixedRadix.size(); i++)
            {
                matrixChecker *= wMixedRadix[i];
            }

            if(matrixChecker!=wBufferSize)
            {
                wMixedRadix.insert(wMixedRadix.begin(), (float)(wBufferSize/matrixChecker));
            }
            
            wRadixSize = (float)wMixedRadix.size();
            xNi.resize(wMixedRadix.size()+0); // Array of variables which are used to prepare array of input indexes
            xNt.resize(wMixedRadix.size()+1); // Array of variables which are used to prepare array of twiddle indexes
            xNo.resize(wMixedRadix.size());   // Array of variables which are used to prepare array of output index
            
            for(int i=0; i<xNt.size(); i++)
            {
                if(i<=1)        xNt[i]=i;
                else if(i==2)   xNt[i] = wMixedRadix[wMixedRadix.size()-1];
                else            xNt[i] = xNt[i-1] * wMixedRadix[wMixedRadix.size()-(i-1)];
                
                if(i<xNi.size())
                {
                    if(i==0)    xNi[0] = 1.0f;
                    else        xNi[i] = xNi[i-1] * wMixedRadix[i-1];
                }
                
                if(i<xNo.size())
                {
                    if(i==0)    xNo[0] = wBufferSize/wMixedRadix[0];
                    else        xNo[i] = xNo[i-1]/wMixedRadix[i];
                }
            }
            sN0.resize((int)wBufferSize); // even temporary input array of each radix //by initial even also stores input signal of buffer size
            sN1.resize((int)wBufferSize); // odd  temporary input array of each radix
        }
    }
}




// == Prepare arrays with components needed to express input and twiddle indexes =============
// ===========================================================================================
void PajFFT_MixedRadix::mixedRadixInputAndTwiddleComponents  (int radixStep)
{
    std::vector<std::vector<int>> tempN0;
    std::vector<std::vector<std::vector<int>>> tempK0;
    
    for(int fft=0; fft<wRadixSize; fft++)
    {
        std::vector<int> tempN1;
        std::vector<std::vector<int>> tempK1;
        
        for(int i=0; i<wMixedRadix[(((wRadixSize-(fft%wRadixSize))%wRadixSize)+radixStep)%wRadixSize]; i++)
        {
            tempN1.push_back(i * xNi[(((wRadixSize-fft)%wRadixSize)+radixStep)%wRadixSize]);
            
            std::vector<int> tempK2;
            for(int j=0; j<wMixedRadix[(((wRadixSize-(fft%wRadixSize))%wRadixSize)+(wMixedRadix.size()-1))%wRadixSize]; j++)
            {
                tempK2.push_back(j * xNi[(wRadixSize-1)-fft] * i * xNt[ fft>=(radixStep+1<wRadixSize?radixStep+1:0) ? (radixStep+1<wRadixSize?fft-radixStep:fft+1) : 0]);
            }
            tempK1.push_back(tempK2);
        }
        tempN0.push_back(tempN1);
        tempK0.push_back(tempK1);
    }
    kIndex.insert(kIndex.begin(), tempK0);
    nIndex.insert(nIndex.begin(), tempN0);
    
    if(radixStep<(wRadixSize-1))
    {
        int shift = radixStep + 1;
        mixedRadixInputAndTwiddleComponents(shift);
    }
}




// == Prepare array that is storing iterators ranges (ends) for each radix ===================
// ===========================================================================================
void PajFFT_MixedRadix::prepareIteratorsBounds               (int radixIterationEnd)
{
    iteratorsEnd[radixIterationEnd].resize(wRadixSize);
    for(int fft=0; fft<wRadixSize; fft++)
    {
        iteratorsEnd[radixIterationEnd][fft] = wMixedRadix[(((wRadixSize-(fft%wRadixSize))%wRadixSize)+radixIterationEnd)%wRadixSize];
    }
    if(radixIterationEnd<wRadixSize-1)
    {
        int nextIterator = radixIterationEnd + 1;
        prepareIteratorsBounds(nextIterator);
    }
}





void PajFFT_MixedRadix::prepareTwiddlesArray                 ()
{
    wnkN_forw.resize(wBufferSize);
    wnkN_back.resize(wBufferSize);

    for(unsigned int i=0; i<wBufferSize; i++)
    {
        wnkN_forw[i] = twiddleCalculator((float)i);
        wnkN_back[i] = 1.0f/(wnkN_forw[i]);
    }
}





void PajFFT_MixedRadix::prepareWindowingArray               ()
{
    windowHann.clear();
    for(int i=0; i<wBufferSize; ++i)
    {
        float windowSample = -0.5*cos(2.*fPi*(double)i/(double)wBufferSize)+0.5;
        windowHann.push_back(windowSample);
    }
}







// =========================================================================================================================================
// == F F T == A L G O R I T H M ===========================================================================================================
// =========================================================================================================================================

// PUBLIC:
// == fft initiation =========================================================================
// ===========================================================================================
void PajFFT_MixedRadix::makeFFT         (std::vector<std::complex<float>> inputSignal, std::vector<std::complex<float>> &wOutputC, bool isForwardOrNot)
{
    std::vector<std::complex<float>>& wnkN = isForwardOrNot?wnkN_forw:wnkN_back;
    isForward=isForwardOrNot;
    
    wOutputData = &wOutputC;
    if(dataPreparedConfirm)
    {
        sN0 = inputSignal;
        
        for(int fft=0; fft<wRadixSize; fft++)
        {
            dftRecursion(fft, 0, wnkN);
        }
    }
    else
    {
        return;
    }
}

void PajFFT_MixedRadix::makeFFT         (std::vector<float> inputSignal, std::vector<std::complex<float>> &wOutputC, bool isForwardOrNot)
{
    std::vector<std::complex<float>>& wnkN = isForwardOrNot?wnkN_forw:wnkN_back;
    isForward=isForwardOrNot;
    
    wOutputData = &wOutputC;
    if(dataPreparedConfirm)
    {
        for(int i=0; i<wBufferSize; i++)
        {
            sN0[i].real(inputSignal[i]);
            sN0[i].imag(0.0f);
        }
        for(int fft=0; fft<wRadixSize; fft++)
        {
            dftRecursion(fft, 0, wnkN);
        }
    }
    else
    {
        return;
    }
}



// PRIVATE:
// == Recursion of DFT for each radix ========================================================
// ===========================================================================================
void PajFFT_MixedRadix::dftRecursion    (int fftss, int radStep, std::vector<std::complex<float>> &twiddle)
{
    int su2 = radStep+1;
    
    for(iteratorsStore[radStep] = 0; iteratorsStore[radStep]<iteratorsEnd[radStep][fftss]; iteratorsStore[radStep]++)
    {
        if(radStep < wRadixSize-2)
        {
            dftRecursion(fftss, su2, twiddle);
        }
        else
        {
            makeDFT(fftss, twiddle);
        }
    }
}




// === Regular DFT or iDFT on the last radix step ============================================
// ===========================================================================================
void PajFFT_MixedRadix::makeDFT         (int wFFT, std::vector<std::complex<float>> &twiddle)
{
    for    (int q=0; q<iteratorsEnd[wRadixSize-1][wFFT]; q++)
    {
        std::complex<float> temp=0.0f;
        
        for(int j=0; j<iteratorsEnd[wRadixSize-1][wFFT]; j++)
        {
            int aaa=nIndex[0][wFFT][j];
            int bbb=kIndex[0][wFFT][q][j];
            for(int x=1; x<wRadixSize; x++)
            {
                aaa += nIndex[x][wFFT][*iteratorsPointer[wRadixSize-x-1]];
                bbb += kIndex[x][wFFT][*iteratorsPointer[wRadixSize-x-1]][j];
            }
            
            if(wFFT%2==0)
            {
                temp += sN0[aaa] * twiddle[bbb%(int)wBufferSize];
            }
            else if(wFFT%2!=0)
            {
                temp += sN1[aaa] * twiddle[bbb%(int)wBufferSize];
            }
        }

        int fff = nIndex[0][wFFT][q];
        for(int x=1; x<wRadixSize; x++)
        {
            fff += nIndex[x][wFFT][*iteratorsPointer[wRadixSize-1-x]];
        }
        
        if(wFFT%2==0)
        {
            sN1[fff] = temp;
        }
        else if(wFFT%2!=0)
        {
            sN0[fff] = temp;
        }
    
        if(wFFT == wRadixSize-1)
        {
            int ggg = xNo[0]*q;
            for(int x=1; x<wRadixSize; x++)
            {
                ggg += xNo[x]*(*iteratorsPointer[x-1]);
            }
            
            if(isForward   &&   ggg>=wBufNyquist)
                wOutputData->at(ggg) = 0.0f;
            else
                wOutputData->at(ggg) = temp;
        }
    }
}







// =========================================================================================================================================
// == C A L C U L A T O R S ================================================================================================================
// =========================================================================================================================================

// PRIVATE:
// == Twiddle calculator =====================================================================
// ===========================================================================================
std::complex<float> PajFFT_MixedRadix::twiddleCalculator        (float nXk)
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




// == Magnitude calculator ===================================================================
// ===========================================================================================
float PajFFT_MixedRadix::freqMagnitudeCalc (std::complex<float> fftOutput, long freqBin)
{
    if(freqBin<lEndScale  ||  freqBin>tEndScale)
        return fZero;
    else
    {
        float re_2;
        float im_2;
        re_2 = fftOutput.real() * fftOutput.real();
        im_2 = fftOutput.imag() * fftOutput.imag();

        return pow(re_2 + im_2, 0.5f);
    }
}

float PajFFT_MixedRadix::waveEnvelopeCalc (std::complex<float> fftOutput, long index, long overLap)
{
    fftOutput *= phaseRotation;
    return windowing(fftOutput.real(), index)/((long)wBufferSize*overLap);
}

float PajFFT_MixedRadix::phaseCalculator          (std::complex<float> fftOutput, long index)
{
    return atan2(fftOutput.imag(),fftOutput.real());
}


std::complex<float> PajFFT_MixedRadix::windowing(std::complex<float> dataToWindowing, long index)
{
    if(isWindowing)
        return dataToWindowing*windowHann[index];
    else
        return dataToWindowing;
}

float PajFFT_MixedRadix::windowing(float dataToWindowing, long index)
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
float              PajFFT_MixedRadix::getBufferSize   ()
{
    return wBufferSize;
}

float              PajFFT_MixedRadix::getSampleRate   ()
{
//    return wSampleRate;
    return 0;
}

int                PajFFT_MixedRadix::getRadDivider   ()
{
    return radixDivider;
}

std::vector<float> PajFFT_MixedRadix::getRadDimensions()
{
    return wMixedRadix;
}


float              PajFFT_MixedRadix::getLowEnd       ()
{
    return low_End;
}

float              PajFFT_MixedRadix::getTopEnd       ()
{
    return top_End;
}

float              PajFFT_MixedRadix::getPhase        ()
{
    return wPhase;
}
