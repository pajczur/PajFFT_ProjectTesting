/*
  ==============================================================================

    GraphAnalyser.cpp
    Created: 16 Apr 2018 11:35:58pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CalculateDTFT.h"

//using namespace pajFFT_HandyFunc;
//==============================================================================
CalculateDTFT::CalculateDTFT()
{
    fPi = 4.0 * atan(1.0);
    dataIsReadyToFFT = false;
    isForward = true;
    fftType = 0;
    isPitchON = false;
    wPitchShift = 1.0f;
    indexFFToutSize = 0;
    indexDEVbufSize = 0;
    dupex=false;
}

CalculateDTFT::~CalculateDTFT()
{
}

void CalculateDTFT::fftCalculator(AudioBuffer<float> &inp)
{
    if(tempOutput.size()>dupa)
        dupex=true;
    
    for(int i=0; i<deviceBuffSize; i++)
    {
        inputDataC[indexFFToutSize] = inp.getSample(0, i);
        tempInput[indexFFToutSize] = inp.getSample(0, i);
        indexFFToutSize++;

        if(indexFFToutSize >= newBufferSize)
        {
            inputData = tempInput;
            fftCalc();
            
            for(int z=0; z<newBufferSize; z++)
            {
                if(isWindowed)
                    tempOutput.push_back(windowedBackFFTout[z]);
                else
                    tempOutput.push_back(backFFTout[z]);
            }
            
            indexFFToutSize = 0;
        }
        
        if(dupex)
        {
            wOutput[i] = tempOutput[0];
            tempOutput.erase(tempOutput.begin());
        }
    }
}

void CalculateDTFT::fftCalculator(std::vector<float> &inp)
{
    if(tempOutput.size()>dupa)
        dupex=true;
    
    for(int i=0; i<deviceBuffSize; i++)
    {
        inputDataC[indexFFToutSize] = inp[i];
        tempInput[indexFFToutSize] = inp[i];
        indexFFToutSize++;

        if(indexFFToutSize >= newBufferSize)
        {
            inputData = tempInput;
            fftCalc();
            
            for(int z=0; z<newBufferSize; z++)
            {
                if(isWindowed)
                    tempOutput.push_back(windowedBackFFTout[z]);
                else
                    tempOutput.push_back(backFFTout[z]);
            }
            
            indexFFToutSize = 0;
        }
        
        if(dupex)
        {
            wOutput[i] = tempOutput[0];
            tempOutput.erase(tempOutput.begin());
        }
    }
}

void CalculateDTFT::getInputData(AudioBuffer<float> &inp)
{
    for(int i=0; i<deviceBuffSize; i++)
    {
        tempInput[indexFFToutSize] = inp.getSample(0, i);
        indexFFToutSize++;
        
        if(indexFFToutSize >= newBufferSize)
        {
            inputData = tempInput;
            indexFFToutSize = 0;
        }
    }
    
}

void CalculateDTFT::fftCalc()
{
    if(dataIsReadyToFFT)
    {
        Clock _time;
    
        switch (fftType)
        {
            case 0:
                return;
                
            case 1:
                if(!isPitchON)
                {
                    if(isForward)
                    {
                        if(isWindowed)
                            windowOverlap_ForwFFT(newBufferSize, 4, wSampleRate, inputDataC);
                        else
                            mixedRadix_FFT.makeFFT(inputDataC, forwFFTout, true);
                    }
                    else
                        if(isWindowed)
                            windowOverlap_ForwBackFFT(newBufferSize, 4, wSampleRate, inputDataC, windowedBackFFTout);
                        else
                        {
                            mixedRadix_FFT.makeFFT(inputDataC, forwFFTout, true);
                            mixedRadix_FFT.makeFFT(forwFFTout, inputDataC, false);
                            for(long i=0; i<newBufferSize; i++)
                            {
                                backFFTout[i] = mixedRadix_FFT.waveEnvelopeCalc(inputDataC[i], i);
                            }
                        }
                }
                else
                {
                    smbPitchShift(wPitchShift, newBufferSize, 4, wSampleRate, inputDataC, windowedBackFFTout);
                }
                break;
                
            case 2:
                if(!isPitchON)
                {
                    if(isForward)
                    {
                        if(isWindowed)
                            windowOverlap_ForwFFT(newBufferSize, 4, wSampleRate, inputDataC);
                        else
                            radix2_FFT.makeFFT(inputDataC, forwFFTout, true);
                    }
                    else
                        if(isWindowed)
                            windowOverlap_ForwBackFFT(newBufferSize, 4, wSampleRate, inputDataC, windowedBackFFTout);
                        else
                        {
                            radix2_FFT.makeFFT(inputDataC, forwFFTout, true);
                            radix2_FFT.makeFFT(forwFFTout, inputDataC, false);
                            for(long i=0; i<newBufferSize; i++)
                            {
                                backFFTout[i] = radix2_FFT.waveEnvelopeCalc(inputDataC[i], i);
                            }
                        }
                }
                else
                {
                    smbPitchShift(wPitchShift, newBufferSize, 4, wSampleRate, inputDataC, windowedBackFFTout);
                }
                break;
                
            case 3:
//                regular_DFT.makeDFT(inputData);
//                if(!isForward)
//                    regular_IDFT.makeDFT(outCompDFT);
                 
                break;
                
            default:
                break;
        }
    
//        std::cout << _time.secondsElapsed() << std::endl;
        timeElapsed = _time.secondsElapsed();
    }
    else
    {
        return;
    }
}

void CalculateDTFT::defineDeviceBuffSize(long dev_buf_size)
{
    deviceBuffSize = dev_buf_size;
    tempOutput.clear();
}

void CalculateDTFT::setNewBufSize(double new_buf_size)
{
    newBufferSize = new_buf_size;
    resetOutputData();
}

void CalculateDTFT::selectFFT(int identifier)
{
    fftType = identifier;
}

void CalculateDTFT::resetOutputData()
{
    indexDEVbufSize=0;
    indexFFToutSize=0;
    inputDataC.resize(newBufferSize);
    inputData.resize(newBufferSize);
    windowedBackFFTout.resize(newBufferSize);
    backFFTout.resize(newBufferSize);
    forwFFTout.resize(newBufferSize);
    freqOutput.resize(newBufferSize);
    tempInput.resize(newBufferSize);
    
    for(int i=0; i<newBufferSize; i++)
    {
        inputData[i] = 0.0f;
        inputDataC[i] = 0.0f;
        windowedBackFFTout[i] = 0.0f;
        backFFTout[i] = 0.0f;
        forwFFTout[i] = 0.0f;
        freqOutput[i] = 0.0f;
        tempInput[i] = 0.0f;
    }
    
    dupa = (ceil(newBufferSize/deviceBuffSize))*deviceBuffSize;
    dupex = false;
    wOutput.resize(deviceBuffSize);


    for(int i=0; i<wOutput.size(); i++)
    {
        wOutput[i] = 0.0f;
    }
    
    
    
    gInFIFO.resize(2*newBufferSize);
    gOutFIFO.resize(2*newBufferSize);
    gFFTworksp.resize(2*newBufferSize);
    outPP.resize(2*newBufferSize);
    outPP2.resize(2*newBufferSize);
    
    for(int i=0; i<2*newBufferSize; i++)
    {
        gFFTworksp[i] = 0.0f;
        outPP[i] = 0.0f;
        outPP2[i] = 0.0f;
        gInFIFO[i] = 0.0f;
        gOutFIFO[i] = 0.0f;
    }
    
    
    
    
    memset(gLastPhase, 0, ((2*newBufferSize)/2+1)*sizeof(float));
    memset(gSumPhase, 0, ((2*newBufferSize)/2+1)*sizeof(float));
    memset(gOutputAccum, 0, (2*newBufferSize)*sizeof(float));
    memset(gAnaFreq, 0, (2*newBufferSize)*sizeof(float));
    memset(gAnaMagn, 0, (2*newBufferSize)*sizeof(float));
    memset(gSynMagn, 0, (2*newBufferSize)*sizeof(float));
    memset(gSynFreq, 0, (2*newBufferSize)*sizeof(float));
}


void CalculateDTFT::smbPitchShift(float pitchShift, long fftFrameSize, long osamp, float sampleRate, std::vector<std::complex<float>> indata, std::vector<float> &outdata)
/*
 I was hardly inspired by:
 Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 */
{
    /* set up some handy variables */
    fftFrameSize2 = fftFrameSize/2;
    stepSize = fftFrameSize/osamp;
    freqPerBin = sampleRate/(double)fftFrameSize;
    expct = 2.*fPi*(double)stepSize/(double)fftFrameSize;
    inFifoLatency = fftFrameSize-stepSize;
    long gRover = inFifoLatency;

    
    /* main processing loop */
    for (int i = 0; i < fftFrameSize; i++){
        
        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover-inFifoLatency].real();
        gRover++;
        
        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;
            
            windowingOverlap_FFT(fftFrameSize);
            
            analyzeData(osamp);
            
            /* ***************** PROCESSING ******************* */
            /* this does the actual pitch shifting */
            memset(gSynMagn, 0, fftFrameSize*sizeof(float));
            memset(gSynFreq, 0, fftFrameSize*sizeof(float));
            for (long k = 0; k <= fftFrameSize2; k++) {
                index = k*pitchShift;
                if (index <= fftFrameSize2) {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index] = gAnaFreq[k] * pitchShift;
                }
            }
            
            /* ***************** SYNTHESIS ******************* */
            /* this is the synthesis step */
            for (long k = 0; k <= fftFrameSize2; k++) {

                magn = gSynMagn[k];
                tmp = gSynFreq[k];

                /* subtract bin mid frequency */
                tmp -= (double)k*freqPerBin;

                /* get bin deviation from freq deviation */
                tmp /= freqPerBin;

                /* take osamp into account */
                tmp = 2.*fPi*tmp/osamp;

                /* add the overlap phase advance back in */
                tmp += (double)k*expct;

                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += tmp;
                phase = gSumPhase[k];
                
                /* get real and imag part and re-interleave */
                forwFFTout[k].real(magn*cos(phase));
                forwFFTout[k].imag(magn*sin(phase));
            }
            
            /* zero negative frequencies */
            for (long k = fftFrameSize2+1; k < fftFrameSize; k++) forwFFTout[k] = 0.0f;
            
            inverseFFT_windowingOverlap(fftFrameSize, osamp);
            
            for (long k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];
            
            /* shift accumulator */
            memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));
            
            /* move input FIFO */
            for (long k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
            
        }
    }
}


void CalculateDTFT::windowOverlap_ForwBackFFT(long fftFrameSize, long osamp, float sampleRate, std::vector<std::complex<float>> indata, std::vector<float> &outdata)
{
    fftFrameSize2 = fftFrameSize/2;
    stepSize = fftFrameSize/osamp;
    freqPerBin = sampleRate/(double)fftFrameSize;
    expct = 2.*fPi*(double)stepSize/(double)fftFrameSize;
    inFifoLatency = fftFrameSize-stepSize;
    long gRover = inFifoLatency;

    for (int i = 0; i < fftFrameSize; i++){

        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover-inFifoLatency].real();
        gRover++;

        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;

            windowingOverlap_FFT(fftFrameSize);

            analyzeData(osamp);

            for (long k = fftFrameSize2+1; k < fftFrameSize; k++) forwFFTout[k] = 0.0f;

            inverseFFT_windowingOverlap(fftFrameSize, osamp);

            for (long k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

            memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));

            for (long k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
        }
    }
}


void CalculateDTFT::windowOverlap_ForwFFT(long fftFrameSize, long osamp, float sampleRate, std::vector<std::complex<float>> indata/*, std::vector<float> &outdata*/)
{
    fftFrameSize2 = fftFrameSize/2;
    stepSize = fftFrameSize/osamp;
    freqPerBin = sampleRate/(double)fftFrameSize;
    expct = 2.*fPi*(double)stepSize/(double)fftFrameSize;
    inFifoLatency = fftFrameSize-stepSize;
    long gRover = inFifoLatency;
    
    for (int i = 0; i < fftFrameSize; i++){
        
        gInFIFO[gRover] = indata[i];
        gRover++;
        
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;
            
            windowingOverlap_FFT(fftFrameSize);
            
            analyzeData(osamp);
            
            for (long k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
        }
    }
}

void CalculateDTFT::windowingOverlap_FFT(long frameSize)
{
    /* do windowing and re,im interleave */
    /* ***************** ANALYSIS ******************* */
    /* do transform */
    if(fftType==1) {
        for (long k = 0; k < frameSize; k++)
        {
            gFFTworksp[k] = mixedRadix_FFT.windowing(gInFIFO[k], k);
        }
        mixedRadix_FFT.makeFFT(gFFTworksp, forwFFTout, true);
    }
    else if(fftType==2) {
        for (long k = 0; k < frameSize; k++)
        {
            gFFTworksp[k] = radix2_FFT.windowing(gInFIFO[k], k);
        }
        radix2_FFT.makeFFT(gFFTworksp, forwFFTout, true);
    }
}


void CalculateDTFT::inverseFFT_windowingOverlap(long frameSize, long overSamp)
{
    /* do inverse transform */
    if(fftType==1)
    {
        mixedRadix_FFT.makeFFT(forwFFTout, outPP2, false);
        /* do windowing and add to output accumulator */
        for(long k=0; k < frameSize; k++) {
            gOutputAccum[k] += 2.0*(mixedRadix_FFT.waveEnvelopeCalc(outPP2[k], k)/overSamp);
        }
    }
    else if(fftType==2)
    {
        radix2_FFT.makeFFT(forwFFTout, outPP2, false);
        /* do windowing and add to output accumulator */
        for(long k=0; k < frameSize; k++) {
            gOutputAccum[k] += 2.0*(radix2_FFT.waveEnvelopeCalc(outPP2[k], k)/overSamp);
        }
    }
}

void CalculateDTFT::analyzeData(long overSamp)
{
    /* this is the analysis step */
    for (long k = 0; k <= fftFrameSize2; k++) {
        
        /* compute magnitude and phase */
        if(fftType==1)
        {
            magn = 2.0*mixedRadix_FFT.freqMagnitudeCalc(forwFFTout[k], k);
            phase = mixedRadix_FFT.phaseCalculator(forwFFTout[k], k);
        }
        else if(fftType==2)
        {
            magn = 2.0*radix2_FFT.freqMagnitudeCalc(forwFFTout[k], k);
            phase = radix2_FFT.phaseCalculator(forwFFTout[k], k);
        }
        
        /* compute phase difference */
        tmp = phase - gLastPhase[k];
        gLastPhase[k] = phase;
        
        /* subtract expected phase difference */
        tmp -= (double)k*expct;
        
        /* map delta phase into +/- Pi interval */
        qpd = tmp/fPi;
        if (qpd >= 0) qpd += qpd&1;
        else          qpd -= qpd&1;
        tmp -= fPi*(double)qpd;
        
        /* get deviation from bin frequency from the +/- Pi interval */
        tmp = overSamp*tmp/(2.*fPi);
        
        /* compute the k-th partials' true frequency */
        tmp = (double)k*freqPerBin + tmp*freqPerBin;
        
        /* store magnitude and true frequency in analysis arrays */
        gAnaMagn[k] = magn;
        gAnaFreq[k] = tmp;
        
        if(!isPitchON) {
            /* subtract bin mid frequency */
            tmp -= (double)k*freqPerBin;
            
            /* get bin deviation from freq deviation */
            tmp /= freqPerBin;
            
            /* take osamp into account */
            tmp = 2.*fPi*tmp/overSamp;
            
            /* add the overlap phase advance back in */
            tmp += (double)k*expct;
            
            /* accumulate delta phase to get bin phase */
            gSumPhase[k] += tmp;
            phase = gSumPhase[k];

            /* get real and imag part and re-interleave */
            forwFFTout[k].real(magn*cos(phase));
            forwFFTout[k].imag(magn*sin(phase));
        }
    }
}



void CalculateDTFT::setSampleRate(float sampR)
{
    wSampleRate = sampR;
    topEnd = wSampleRate;
}

void CalculateDTFT::setLowEnd(float lowE)
{
    lowEnd = newBufferSize * (lowE/wSampleRate);
}

void CalculateDTFT::setTopEnd(float topE)
{
    topEnd = newBufferSize * (topE/wSampleRate);
}
