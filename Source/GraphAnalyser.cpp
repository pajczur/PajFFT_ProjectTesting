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
    isForward = true;
}

GraphAnalyser::~GraphAnalyser()
{
}

void GraphAnalyser::paint (Graphics& g)
{
    g.setColour (Colours::red);
    
    if(!fftGraph.isEmpty())
    {
        g.strokePath(fftGraph, PathStrokeType(2));
    }
}

void GraphAnalyser::resized()
{
}


void GraphAnalyser::timerCallback()
{
    if(dataSource->fftType != 0)
    {
        if(!dataSource->wOutput->empty())
        {
            fftGraph.clear();
            
            if(!isForward)
            {
                drawLinearGraph();
            }
            else
            {
                drawLogarGraph3();
            }

            repaint();
        }
    }
    else
    {
        if(!fftGraph.isEmpty())
            fftGraph.clear();
        repaint();
    }
}


void GraphAnalyser::drawLinearGraph()
{
    if(dataSource->wOutput->size()>1)
    {
        fftGraph.startNewSubPath(0, -(dataSource->wOutput->at((int)timeStart%(dataSource->wOutput->size()-1)) * zero_dB/2.0) + (zero_dB/2.0));
        
        double avarage = 0.0;
        for(float i=timeStart+1.0; i<=wSampleRate; i++)
        {
            avarage += dataSource->wOutput->at((int)i%(dataSource->wOutput->size()-1));
            
            if(fmod(round(i), linearDivider)==0)
            {
                avarage = avarage/linearDivider;
                double wCurrent   = dispWidth * ((i)-timeStart);
                fftGraph.lineTo(wCurrent, -(avarage * zero_dB/2.0) + (zero_dB/2.0));
                avarage = 0.0;
            }
        }
    }
    else return;
}

void GraphAnalyser::drawLogarGraph3()
{
    if(dataSource->wOutput->size()>1)
    {
        fftGraph.startNewSubPath(0, -(dataSource->wOutput->at(low_End_index) * zero_dB) + zero_dB);

        float tempCurr=0.0;

        for(float i=low_End_index+1.0f; i<=dataSource->wOutput->size()/2.0f; i++)
        {

            double wBefore    = dispLogScale * (log10((i-1.0)*logScaleWidth1) - log10(low_End));
            double wCurrent   = dispLogScale * (log10((i-0.0)*logScaleWidth1) - log10(low_End));

            tempCurr+=dataSource->wOutput->at(i);

            if(round(wCurrent) != round(wBefore))
            {
                fftGraph.lineTo(wCurrent, -(tempCurr * zero_dB) + zero_dB);

                tempCurr=0.0;

            }
        }
    }
    else return;
}

void GraphAnalyser::setSampleRate(double sample_rate)
{
    wSampleRate = sample_rate;
    nyquist = wSampleRate/2.0;
    zero_dB = (float)getHeight()-1.0f;
    logNyquist = log10(nyquist);
    low_End = 10.0f;
    top_End = nyquist;
    low_End_index = round(1.0f * ( newBufferSize / wSampleRate));
    timeStart = 0.0f;
    timeEnd = wSampleRate;
    dispWidth = (double)getWidth() / (timeEnd - timeStart);
    linearDivider = floor(wSampleRate/1000.0f);
}

void GraphAnalyser::setNewBufSize(double new_buf_size)
{
    newBufferSize = new_buf_size;
    low_End_index = round(low_End * ( newBufferSize / wSampleRate));
    top_End_index = round(top_End * ( newBufferSize / wSampleRate));
    //    dispResol = ((maxResolution>=(newBufferSize/2.0))?(newBufferSize/2.0):maxResolution);
    logScaleWidth1 = nyquist/(newBufferSize/2.0);
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
    
    linearDivider = floor((endTime - startTime)/1000);
}

void GraphAnalyser::setLowEndIndex()
{
    low_End_index = round(1.0f * ( newBufferSize / wSampleRate));
}

void GraphAnalyser::setFFT_DataSource(CalculateDTFT &fftData)
{
    dataSource = &fftData;
}
