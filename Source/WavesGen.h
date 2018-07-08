/*
  ==============================================================================

    WavesGen.h
    Created: 17 Apr 2018 7:53:29pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class WavesGen
{
public:
    WavesGen();
    ~WavesGen();
    void setSampleRate(double sampRate);
    int getWaveType();
    
private:
    void createWaveTables(double sampRate);
    
    double sinWave();
    double sawWave();
    double squereWave ();
    double wNoise();
    
    
    double oscillator(int wave1234);
    
public:
    void squereWaveAsymetric (double shift);
    
    void setFrequency(int freq);
    
    void setAmplitude(float amp);
    
    void selectWave(int wave0123);
    
    
    void playWave(AudioBuffer<float> &outputBuffer, float bufferSize, int startSample);
    void prepareWave(std::vector<float> &outputBuffer, float bufferSize, int startSample);

    double dPi;
private:
    double wSampleRate;
    double wBufferSize;
    int phase;
    double frequency;
    double squereWaveSymetry;
    Array<double> sinwave;
    Array<double> sawwave;
    Array<double> sqrwave;
    double increment;
    int waveType;
    float amplitude;


    float slideTempUp();
    float slideTempDown();
    float slide=0.0;
};
