/*
  ==============================================================================

    WavesGen.cpp
    Created: 17 Apr 2018 7:53:29pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WavesGen.h"
#include <cmath>
#include <iostream>


//==============================================================================
WavesGen::WavesGen()
{
    dPi = 4*atan(1.0);
    wSampleRate = 0.0;
    phase = 0.0;
    squereWaveSymetry = 0.5;
    waveType = 0;
    amplitude = 0.0;
}

WavesGen::~WavesGen()
{
}

void WavesGen::setSampleRate(double sampRate)
{
    wSampleRate = sampRate;
    createWaveTables(sampRate);
}


void WavesGen::createWaveTables(double sampRate)
{
    sinwave.clear();
    sawwave.clear();
    sqrwave.clear();
    for(int i=0; i<sampRate; i++)
    {
        double tempSAW = 0;
        double tempSQRT = 0;
        for(int n=1; n<100; n++)
        {
            tempSAW        += sin((double)n*2.0*dPi*(double)i/sampRate) / (double)n;
            tempSQRT       += sin(2*dPi*((2.0*(double)n)-1.0)*(double)i/sampRate) / (double)((2*n)-1);
        }
        sinwave.insert(i, sin(2.0*dPi*(double)i/sampRate));
        sawwave.insert(i, tempSAW);
        sqrwave.insert(i, (dPi/4.0)*tempSQRT);
    }
}

double WavesGen::sinWave()
{
    return(sinwave[phase]);
}


double WavesGen::sawWave()
{
    return(sawwave[phase]);
}


double WavesGen::squereWave()
{
    return(sqrwave[phase]);
}

void WavesGen::squereWaveAsymetric(double shift)
{
    if((shift > 1.0/wSampleRate) && (shift <(wSampleRate-1.0)/wSampleRate))
        squereWaveSymetry = shift;
    else if (shift < 1.0/wSampleRate)
        squereWaveSymetry = 1.0/wSampleRate;
    else
        squereWaveSymetry = (wSampleRate-1.0)/wSampleRate;
}

double WavesGen::wNoise()
{
    double whitenoise = (rand()/(double)RAND_MAX);
    return(whitenoise * 2.0 - 1.0);
}

void WavesGen::setFrequency(int freq)
{
    frequency = freq;
}

void WavesGen::setAmplitude(float amp)
{
    amplitude = amp;
}

void WavesGen::selectWave(int wave0123)
{
    waveType = wave0123;
}

double WavesGen::oscillator(int wave0123)
{
    switch (wave0123)
    {
        case 0:
            return 0.0;
            
        case 1:
            return sinWave();

        case 2:
            return sawWave();

        case 3:
            return squereWave();
            
        case 4:
            return wNoise();

        default:
            return sinWave();
    }
}



void WavesGen::playWave(AudioBuffer<float> &outputBuffer, float bufSize, int startSample)
{
    for (int sample=0; sample<bufSize; sample++)
    {
        if(phase>=wSampleRate) phase-=wSampleRate;
        
        double voice = oscillator(waveType) * amplitude;
        
        outputBuffer.addSample(0, startSample, voice);
        outputBuffer.addSample(1, startSample, voice);
        startSample++;
        phase += frequency;
    }
}

void WavesGen::prepareWave(std::vector<float> &outputBuffer, float bufSize, int startSample)
{
    outputBuffer.clear();
    for (int sample=0; sample<bufSize; sample++)
    {
        if(phase>=wSampleRate) phase-=wSampleRate;
        
        double voice = oscillator(waveType) * amplitude;
        
        outputBuffer.push_back(voice);

        startSample++;
        phase += frequency;
    }
}


int WavesGen::getWaveType()
{
    return waveType;
}
