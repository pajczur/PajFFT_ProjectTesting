/*
  ==============================================================================

    GraphAnalyser.cpp
    Created: 16 Apr 2018 11:35:58pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CalculateDTFT.h"

//==============================================================================
CalculateDTFT::CalculateDTFT()
{
    dataIsReadyToFFT = false;
    isForward = true;
    fftType = 0;
    isPitchON = false;
    wPitchShift = 1.0f;
    indeX = 0;
}

CalculateDTFT::~CalculateDTFT()
{
}

void CalculateDTFT::setInputData(AudioBuffer<float> &inp)
{
    for(int i=0; i<inp.getNumSamples(); i++)
    {
        inputDataC[indeX] = inp.getSample(0, i);
        indeX++;

        if(indeX >= (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            indeX = 0;
            fftCalc();
        }
    }
}

void CalculateDTFT::setInputData(std::vector<float> &inp)
{
    for(int i=0; i<inp.size(); i++)
    {
        inputDataC[indeX] = inp[i];
        indeX++;

        if(indeX >= (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            indeX = 0;
            fftCalc();
        }
    }
}

void CalculateDTFT::setOutputData(std::vector<float> &outp)
{
    
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
                        mixedRadix_FFT.makeFFT(inputDataC, inputDataC, true);
                        for(long i=0; i<newBufferSize/2; i++)
                        {
                            outRealMixed[i] = mixedRadix_FFT.freqMagnitudeCalc(inputDataC[i], i)/(newBufferSize/2);
                        }
                    }
                    else
                    {
                        mixedRadix_FFT.makeFFT(inputDataC, inputDataC, true);
                        mixedRadix_FFT.makeFFT(inputDataC, inputDataC, false);
                        for(long i=0; i<newBufferSize; i++)
                        {
                            outRealMixed[i] = mixedRadix_FFT.waveEnvelopeCalc(inputDataC[i], i, 1);
                        }
                    }
                }
                else
                    smbPitchShift2(wPitchShift, newBufferSize, newBufferSize, 4, 44100.0f, inputDataC, outRealMixed);
                break;
                
            case 2:
//                radix2_FFT.makeFFT(inputData);
//                if(!isForward)
//                    radix2_IFFT.makeFFT(outCompRadix2);
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

void CalculateDTFT::setNewBufSize(double new_buf_size)
{
    newBufferSize = new_buf_size;
}

void CalculateDTFT::setRadix2BuffSize(double buf_size)
{
    radix2BuffSize = buf_size;
}

void CalculateDTFT::selectFFT(int identifier)
{
    fftType = identifier;
}

void CalculateDTFT::resetOutputData()
{
    outCompMixed.resize(newBufferSize);
    inputDataC.resize(newBufferSize);
    outRealMixed.resize(newBufferSize);
    for(int i=0; i<newBufferSize; i++)
    {
//        inputData[i] = 0.0f;
        inputDataC[i] = 0.0f;
        outRealMixed[i] = 0.0f;
    }
    
    
    gInFIFO.resize(MAX_FRAME_LENGTH);
    gOutFIFO.resize(MAX_FRAME_LENGTH);
    gFFTworksp.resize(2*MAX_FRAME_LENGTH);
    outPP.resize(2*MAX_FRAME_LENGTH);
    outPP2.resize(2*MAX_FRAME_LENGTH);
    
    for(int i=0; i<2*MAX_FRAME_LENGTH; i++)
    {
        gFFTworksp[i] = 0.0f;
        outPP[i] = 0.0f;
        outPP2[i] = 0.0f;
        if(i<MAX_FRAME_LENGTH)
        {
            gInFIFO[i] = 0.0f;
            gOutFIFO[i] = 0.0f;
        }
    }
    memset(gLastPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
    memset(gSumPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
    memset(gOutputAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
    memset(gAnaFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
    memset(gAnaMagn, 0, MAX_FRAME_LENGTH*sizeof(float));
    memset(gSynMagn, 0, MAX_FRAME_LENGTH*sizeof(float));
    memset(gSynFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
    
    
//    for(int i=0; i<wOutput->size(); i++)
//    {
//        wOutput->at(i) = 0.0f;
//    }
}


void CalculateDTFT::smbPitchShift2(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, std::vector<std::complex<float>> indata, std::vector<float> &outdata)
/*
 Routine smbPitchShift(). See top of file for explanation
 Purpose: doing pitch shifting while maintaining duration using the Short
 Time Fourier Transform.
 Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 */
{
    /* set up some handy variables */
    gRover=false;
    fftFrameSize2 = fftFrameSize/2;
    stepSize = fftFrameSize/osamp;
    freqPerBin = sampleRate/(double)fftFrameSize;
    expct = 2.*M_PI*(double)stepSize/(double)fftFrameSize;
    inFifoLatency = fftFrameSize-stepSize;
    if (gRover == false) gRover = inFifoLatency;

    
    /* main processing loop */
    for (int i = 0; i < numSampsToProcess; i++){
        
        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover-inFifoLatency].real();
        gRover++;
        
        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;
            
            /* do windowing and re,im interleave */
            for (long k = 0; k < fftFrameSize;k++)
            {
                gFFTworksp[k] = mixedRadix_FFT.windowing(gInFIFO[k], k);
            }
            
            
            /* ***************** ANALYSIS ******************* */
            /* do transform */
            mixedRadix_FFT.makeFFT(gFFTworksp, outPP, true);
            
            /* this is the analysis step */
            for (long k = 0; k <= fftFrameSize2; k++) {
                
                /* compute magnitude and phase */
                magn = 2.0*mixedRadix_FFT.freqMagnitudeCalc(outPP[k], k);
                phase = mixedRadix_FFT.phaseCalculator(outPP[k], k);
                
                /* compute phase difference */
                tmp = phase - gLastPhase[k];
                gLastPhase[k] = phase;
                
                /* subtract expected phase difference */
                tmp -= (double)k*expct;
                
                /* map delta phase into +/- Pi interval */
                qpd = tmp/M_PI;
                if (qpd >= 0) qpd += qpd&1;
                else qpd -= qpd&1;
                tmp -= M_PI*(double)qpd;
                
                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = osamp*tmp/(2.*M_PI);
                
                /* compute the k-th partials' true frequency */
                tmp = (double)k*freqPerBin + tmp*freqPerBin;
                
                /* store magnitude and true frequency in analysis arrays */
                gAnaMagn[k] = magn;
                gAnaFreq[k] = tmp;
            }
            
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
                
                /* get magnitude and true frequency from synthesis arrays */
                magn = gSynMagn[k];
                tmp = gSynFreq[k];
                
                /* subtract bin mid frequency */
                tmp -= (double)k*freqPerBin;
                
                /* get bin deviation from freq deviation */
                tmp /= freqPerBin;
                
                /* take osamp into account */
                tmp = 2.*M_PI*tmp/osamp;
                
                /* add the overlap phase advance back in */
                tmp += (double)k*expct;
                
                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += tmp;
                phase = gSumPhase[k];
                
                /* get real and imag part and re-interleave */
                gFFTworksp[k].real(magn*cos(phase));
                gFFTworksp[k].imag(magn*sin(phase));
            }
            
            /* zero negative frequencies */
            for (long k = fftFrameSize+2; k < 2*fftFrameSize; k++) gFFTworksp[k] = 0.0f;
            
            /* do inverse transform */
            mixedRadix_FFT.makeFFT(gFFTworksp, outPP2, false);
            
            
            /* do windowing and add to output accumulator */
            for(long k=0; k < fftFrameSize; k++) {
                gOutputAccum[k] += 2.0*mixedRadix_FFT.waveEnvelopeCalc(outPP2[k], k, osamp);
            }
            
            for (long k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];
            
            /* shift accumulator */
            memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));
            
            /* move input FIFO */
            for (long k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
            
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
