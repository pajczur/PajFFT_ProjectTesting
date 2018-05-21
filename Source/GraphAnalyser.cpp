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
    gg=2.536 * pow(10.0, -5.0);
}

GraphAnalyser::~GraphAnalyser()
{
}

void GraphAnalyser::paint (Graphics& g)
{
    g.setColour (Colours::red);
    
    if(!fftGraph.isEmpty() && dataSource->fftType != 0)
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
        if(!dataSource->outRealMixed.empty())
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
        stopTimer();
        if(!fftGraph.isEmpty())
        {
            fftGraph.clear();
            repaint();
        }
    }
    
}


void GraphAnalyser::drawLinearGraph()
{
    int startPoint = (int)timeStart%(int)(newBufferSize-1);
    fftGraph.startNewSubPath(0, -(dataSource->outRealMixed[startPoint] * zero_dB/2.0) + (zero_dB/2.0));
    
    double avarage = 0.0;
    for(float i=timeStart+1.0; i<=wSampleRate; i++)
    {
        avarage += dataSource->outRealMixed[(int)i%(int)(newBufferSize-1)];
        
        if(fmod(round(i), linearDivider)==0)
        {
            avarage = avarage/linearDivider;
            double wCurrent   = dispWidth * ((i)-timeStart);
            fftGraph.lineTo(wCurrent, -(avarage * zero_dB/2.0) + (zero_dB/2.0));
            avarage = 0.0;
        }
    }
}

void GraphAnalyser::drawLogarGraph3()
{
    double logaa;
    double kkk= 0.0;
    
    if(dataSource->outRealMixed[low_End_index] <= 0.016)
        logaa = 0.0;
    else
        logaa = (   ( /*a_weighting(low_End_index) */ 20.0 * log10(dataSource->outRealMixed[low_End_index]))       +36.0 )  /36.0;
    
//    fftGraph.startNewSubPath(0, -(dataSource->outRealMixed[low_End_index] * zero_dB) + zero_dB);
    fftGraph.startNewSubPath(0, -(dataSource->outRealMixed[low_End_index] * zero_dB) + zero_dB);

    float tempCurr=0.0;

    for(float i=low_End_index+1.0f; i<=buffNyquist; i++)
    {
        kkk++;
        double wBefore    = dispLogScale * (log10((i-1.0)*logScaleWidth1) - log10(low_End));
        double wCurrent   = dispLogScale * (log10((i-0.0)*logScaleWidth1) - log10(low_End));

        if(dataSource->outRealMixed[i] <= 0.016)
            logaa = 0.0;
        else
            logaa = (   ( /*a_weighting(i)*/ 20.0 * log10(dataSource->outRealMixed[i]))       +36.0 )  /36.0;
        
//        tempCurr+=(dataSource->outRealMixed[i] * (dupsko==0.0 ? 1.0 : dupsko));
        tempCurr+=( logaa  );

        if(round(wCurrent) != round(wBefore))
        {
            fftGraph.lineTo(wCurrent, -(tempCurr * zero_dB * 0.75/kkk) + zero_dB);

            tempCurr=0.0;
            kkk=0.0;
        }
    }
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
    buffNyquist = newBufferSize/2.0;
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
    
    linearDivider = floor((endTime - startTime)/maxResolution);
}

void GraphAnalyser::setLowEndIndex()
{
    low_End_index = round(1.0f * ( newBufferSize / wSampleRate));
}

void GraphAnalyser::setFFT_DataSource(CalculateDTFT &fftData)
{
    dataSource = &fftData;
}

void GraphAnalyser::clearDisplay()
{
    fftGraph.clear();
    repaint();
}


double GraphAnalyser::f_weighting(int freqIndex)
{
    double iTof = (double)freqIndex * 22.049 / (double)buffNyquist;
    double z1 = pow(iTof, 2.0);
    double z2 = pow(pow(0.58, 2.0) + pow(1.03, 2.0) - z1, 2.0) + 4*pow(0.58, 2.0)*z1;
    double z3 = pow(pow(3.18, 2.0) + pow(8.75, 2.0) - z1, 2.0) + 4*pow(3.18, 2.0)*z1;
    double p1 = pow(0.18, 2.0) + z1;
    double p2 = pow(1.63, 2.0) + z1;
    double p3 = pow(pow(2.51, 2.0) + pow(3.85, 2.0) - z1, 2.0) + 4*pow(2.51, 2.0)*z1;
    double p4 = pow(pow(6.62, 2.0) + pow(14.29,2.0) - z1, 2.0) + 4*pow(6.62, 2.0)*z1;

    double dupsko = ( gg * pow(z1, 3.0)*z2*pow(z3, 3.0) / (pow(p1, 3.0) * pow(p2, 2.0) * pow(p3, 4.0))) * ( pow(pow(10.0, 5.0)/p4, 20.0) );
    
    return dupsko;
}

double GraphAnalyser::a_weighting(int freqIndex)
{
    double iTof = (double)freqIndex * 22049 / (double)buffNyquist;
    double f_2 = pow(iTof, 2.0);
    double z1 = pow(12194.0, 2.0);
    double z2 = f_2 + pow(20.6, 2.0);
    double z3 = pow(     (f_2 + pow(107.7, 2.0)) * (f_2 + pow(737.9, 2.0)), 0.5);
    double z4 = f_2 + pow(12194.0, 2.0);
    
    
    double dupsko = (z1 * f_2 * f_2) / (z2 * z3 * z4);
    
    return dupsko;
}
