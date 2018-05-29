/*
  ==============================================================================

    GraphAnalyser.cpp
    Created: 9 May 2018 5:56:23pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "GraphAnalyser.h"

//==============================================================================
GraphAnalyser::GraphAnalyser()
{
    wSampleRate = 0.0;
    maxResolution = 1000;
    isFreqAnalyser = true;
    isDWeighting = false;
    isFFTon=false;
}

GraphAnalyser::~GraphAnalyser()
{
}

void GraphAnalyser::paint (Graphics& g)
{
    g.setColour (Colours::red);

    if(!isFreqAnalyser)
        g.strokePath(wavGraph, PathStrokeType(2));
    else
        g.strokePath(fftGraph, PathStrokeType(2));
}

void GraphAnalyser::resized()
{
}


void GraphAnalyser::timerCallback()
{
    
    if(oscilSource->getWaveType() != 0   ||   audioSource->transportSource.isPlaying())
    {
        if(!dataSource->backFFTout.empty())
        {
            
            if(!isFreqAnalyser)
            {
                wavGraph.clear();
                drawLinGraph();
            }
            else
            {
                fftGraph.clear();
                if(isFFTon)
                    drawLogGraph();
            }
            
            repaint();
        }
    }
    else
    {
        stopTimer();
//        if(!fftGraph.isEmpty())
            clearDisplay();
    }
    
}


void GraphAnalyser::drawLinGraph()
{
    std::vector<float> env;
    if(dataSource->isForward)
    {
        env = dataSource->inputData;
    }
    else
    {
        env.clear();
        env = dataSource->backFFTout;
    }
    
    int startPoint = (int)timeStart%(int)(wBufferSize-1);
    wavGraph.startNewSubPath(0, -(env[startPoint] * zero_dB/2.0) + (zero_dB/2.0));
    
    double avarage = 0.0;
    for(float i=timeStart+1.0; i<=wSampleRate; i++)
    {
        if(dataSource->isWindowed   &&   !dataSource->isForward)
            avarage += dataSource->windowedBackFFTout[(int)i%(int)(wBufferSize-1)];
        else
            avarage += env[(int)i%(int)(wBufferSize-1)];

        if(fmod(round(i), linearDivider)==0)
        {
            avarage = avarage/linearDivider;
            double wCurrent   = dispWidth * ((i)-timeStart);
            wavGraph.lineTo(wCurrent, -(avarage * zero_dB/2.0) + (zero_dB/2.0));
            avarage = 0.0;
        }
    }
}

void GraphAnalyser::drawLogGraph()
{
    float linearMagnitude=abs(dataSource->forwFFTout[low_End_index].real()) / (wBufferSize/2.0f);

    fftGraph.startNewSubPath(low_End_index, wDecibels(linearMagnitude, low_End_index) );
    
    linearMagnitude=0.0;

    for(float i=low_End_index+1.0f; i<=buffNyquist; i++)
    {
        double wBefore    = dispLogScale * (log10((i-1.0)*logScaleWidth) - log10(low_End));
        double wCurrent   = dispLogScale * (log10((i-0.0)*logScaleWidth) - log10(low_End));

        if(linearMagnitude<=abs(dataSource->forwFFTout[i].real())/ (wBufferSize/2.0f))
            linearMagnitude =  abs(dataSource->forwFFTout[i].real())  / (wBufferSize/2.0f);

        if(round(wCurrent) != round(wBefore))
        {
            fftGraph.lineTo(wCurrent, wDecibels(linearMagnitude, i));
            linearMagnitude=0.0;
        }
    }
}







void GraphAnalyser::setSampleRate(double sample_rate)
{
    wSampleRate = sample_rate;
    nyquist = wSampleRate/2.0;
    zero_dB = (float)getHeight()-1.0f;
    low_End = 10.0f;
    top_End = nyquist;
    low_End_index = round(1.0f * ( wBufferSize / wSampleRate));
    timeStart = 0.0f;
    timeEnd = wSampleRate;
    dispWidth = (double)getWidth() / (timeEnd - timeStart);
    linearDivider = floor(wSampleRate/1000.0f);
}

void GraphAnalyser::setNewBufSize(double new_buf_size)
{
    wBufferSize = new_buf_size;
    buffNyquist = wBufferSize/2.0;
    low_End_index = round(low_End * ( wBufferSize / wSampleRate));
    logScaleWidth = nyquist/(wBufferSize/2.0);
    dispLogScale = (double)getWidth()/( log10(top_End) - log10(low_End));
}

void GraphAnalyser::setZoomLogar(double lowE, double topE)
{
    top_End = topE;
    low_End = lowE;
    if(log10(topE) - log10(lowE) == 0.0)
        dispLogScale = 0.0;
    else
        dispLogScale = ( (double)getWidth() ) / ( log10(topE) - log10(lowE));
}

void GraphAnalyser::setZoomLinear(double startTime, double endTime)
{
    timeStart = startTime;
    timeEnd = endTime;
    
    if((endTime - startTime)-1.0 == 0.0)
        dispWidth = 0.0f;
    else
        dispWidth = (double)getWidth() / ((endTime - startTime)-1.0);
    
    linearDivider = floor((endTime - startTime)/maxResolution);
}

void GraphAnalyser::setLowEndIndex()
{
    low_End_index = round(1.0f * ( wBufferSize / wSampleRate));
}

void GraphAnalyser::setFFT_DataSource(CalculateDTFT &fftData,  WavesGen &oscData, AudioPlayer &audioData)
{
    dataSource = &fftData;
    oscilSource = &oscData;
    audioSource = &audioData;
}

void GraphAnalyser::clearDisplay()
{
    fftGraph.clear();
    repaint();
}


double GraphAnalyser::a_weighting(int freqIndex)
{
    double frInd = (double)freqIndex;
    if(frInd<0.00026)
        frInd = 0.00001;
    
    double iTof = frInd * 22050 / (double)buffNyquist;
    double f_2 = pow(iTof, 2.0);
    double z1 = pow(12194.0, 2.0);
    double z2 = f_2 + pow(20.6, 2.0);
    double z3 = pow(     (f_2 + pow(107.7, 2.0)) * (f_2 + pow(737.9, 2.0)), 0.5);
    double z4 = f_2 + pow(12194.0, 2.0);
    
    double dupsko = ((log10( (z1 * f_2 * f_2) / (z2 * z3 * z4) ) * 20.0) + 2.0 + 72.0) / 72.0;
    return dupsko;
}

double GraphAnalyser::d_weighting(int freqIndex)
{
    double frInd = (double)freqIndex;
    if(frInd<0.00026)
        frInd = 0.00001;
    
    double iTof = frInd * 22050 / (double)buffNyquist;
    double fr   = iTof;
    double fr_2 = pow(fr, 2.0);
    double z1 = 6.8966888496476 * pow(10.0, -5.0);
    double z2 = (fr_2 + 79919.29) * (fr_2 + 1345600);
    double hf = (pow(1037918.48 - fr_2, 2.0) + (1080768.16 * fr_2)) / ( pow(9837328 - fr_2, 2.0) + (11723776 * fr_2) );
    
    double dupsko = ((log10(  (fr/z1) * pow(hf/z2, 0.5) ) * 20.0) + 72.0) / 72.0;
    return dupsko;
}


double GraphAnalyser::wDecibels(double linearMag, int freqIndex)
{
    float dweight;
    
    if(isDWeighting)
    {
        dweight = d_weighting(freqIndex);
        
        if(linearMag <= 0.0001)
        {
            linearMag = 0.00001;
            dweight = 0.00001f;
        }
    }
    else
    {
        dweight = 1.0f;
        if(linearMag <= 0.0001)
        {
            linearMag = 0.00001;
        }
    }
    
    return  -(0.75 * zero_dB * dweight * (20.0*log10(linearMag) + 72.0)/72.0) + zero_dB;
}
