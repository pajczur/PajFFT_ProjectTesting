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
    inputD.clear();
    dataIsInUse = false;
    dataIsReadyToFFT = false;
    wSampleRate = 0.0;
    maxResolution = 1000;
    isForward = true;
    dataIsAfterFFT = false;
    temm=0;
    oneOfTwo = true;
}

CalculateDTFT::~CalculateDTFT()
{
}

void CalculateDTFT::paint (Graphics& g)
{
    g.setColour (Colours::red);

    if(!fftGraph.isEmpty())
    {
        g.strokePath(fftGraph, PathStrokeType(2));
    }
}

void CalculateDTFT::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void CalculateDTFT::setInputData(AudioBuffer<float> &inp)
{
    for(int i=0; i<inp.getNumSamples(); i++)
    {
        if(inputD.size()< (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            inputD.push_back(inp.getSample(0, i));
        }
        else
        {
            if(!inputD.empty())
            {
                dataIsInUse = true;
                dataIsReadyToFFT = true;
            }
            break;
        }
    }
}

void CalculateDTFT::setInputData(std::vector<float> &inp)
{
    for(int i=0; i<inp.size(); i++)
    {
        if(inputD.size()< (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            inputD.push_back(inp[i]);
        }
        else
        {
            if(!inputD.empty())
            {
                dataIsInUse = true;
                dataIsReadyToFFT = true;
            }
            break;
        }
    }
}

void CalculateDTFT::setOutputData(std::vector<float> &outp)
{
    
}

void CalculateDTFT::timerCallback()
{
    if(dataIsReadyToFFT)
    {
//        stopTimer();
        dataIsReadyToFFT = false;
        Clock _time;
        switch (fftType)
        {
            case 0:
                return;

            case 1:
                mixedRadix_FFT.makeFFT(inputD);
                if(!isForward)
                {
                    mixedRadix_IFFT.makeFFT(outCompMixed);
//                    if(oneOfTwo)
//                    {
//                        mixedRadix_IFFT.setOutputAddress(outRealMixed_1);
//                        oneOfTwo = false;
//                    }
//                    else
//                    {
//                        mixedRadix_IFFT.setOutputAddress(outRealMixed_2);
//                        oneOfTwo = true;
//                    }
                }
//                if(oneOfTwo)
//                {
//                    dataIsAfterFFT = true;
//                }
                break;

            case 2:
                radix2_FFT.makeFFT(inputD);
                if(!isForward)
                    radix2_IFFT.makeFFT(outCompRadix2);
                break;
                
            case 3:
                regular_DFT.makeDFT(inputD);
                if(!isForward)
                    regular_IDFT.makeDFT(outCompDFT);
                break;

            default:
                break;
        }
//        std::cout << _time.secondsElapsed() << std::endl;
        fftGraph.clear();
        if(!isForward)
        {
            drawGraph();
        }
        else
        {
            drawGraph3();
        }
        repaint();
        
        inputD.clear();
        dataIsInUse = false;
        

//        startTimer(ceil((newBufferSize/wSampleRate)*1000));
    }
    else
    {
        return;
    }
}

void CalculateDTFT::drawGraph()
{
    if(!wOutput1->empty())
    {
        fftGraph.startNewSubPath(0, -(wOutput1->at((int)timeStart%(wOutput1->size()-1)) * zero_dB/2.0) + (zero_dB/2.0));

        double avarage = 0.0;
        for(float i=timeStart+1.0; i<=wSampleRate; i++)
        {
            avarage += wOutput1->at((int)i%(wOutput1->size()-1));

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

void CalculateDTFT::drawGraph3()
{
    if(wOutput1->size()>1)
    {
//        fftGraph.startNewSubPath(0, -(wOutput->at(low_End_index) * zero_dB) + zero_dB);
        fftGraph.startNewSubPath(0, -(wOutput1->at(low_End_index) * zero_dB) + zero_dB);

        float tempCurr=0.0;

        for(float i=low_End_index+1.0f; i<=wOutput1->size()/2.0f; i++)
//        for(float i=low_End_index+1.0f; i<=top_End_index; i++)
        {
            
            double wBefore    = dispLogScale * (log10((i-1.0)*logScaleWidth1) - log10(low_End));
            double wCurrent   = dispLogScale * (log10((i-0.0)*logScaleWidth1) - log10(low_End));

            tempCurr+=wOutput1->at(i);

            if(round(wCurrent) != round(wBefore))
            {
                fftGraph.lineTo(wCurrent, -(tempCurr * zero_dB) + zero_dB);
    //            fftGraph.quadraticTo(((wCurrent-wBefore)/2.0)+wBefore, zero_dB, wCurrent, -(tempCurr * zero_dB) + zero_dB);
                tempCurr=0.0;

            }
        }
    }
    else return;
}

void CalculateDTFT::setSampleRate(double sample_rate)
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

void CalculateDTFT::setNewBufSize(double new_buf_size)
{
    newBufferSize = new_buf_size;
    low_End_index = round(low_End * ( newBufferSize / wSampleRate));
    top_End_index = round(top_End * ( newBufferSize / wSampleRate));
//    dispResol = ((maxResolution>=(newBufferSize/2.0))?(newBufferSize/2.0):maxResolution);
    logScaleWidth1 = nyquist/(newBufferSize/2.0);
    dispLogScale = (double)getWidth()/( log10(top_End) - log10(low_End));
}

void CalculateDTFT::setZoomLogar(double lowE, double topE)
{
    top_End = topE;
    low_End = lowE;
    if(log10(topE) - log10(lowE) == 0.0)
        dispLogScale = 0.0;
    else
        dispLogScale = ( (double)getWidth() ) / ( log10(topE) - log10(lowE));
    

}

void CalculateDTFT::setZoomLinear(double startTime, double endTime)
{
    timeStart = startTime;
    timeEnd = endTime;

    if((endTime - startTime)-1.0 == 0.0)
       dispWidth = 0.0f;
   else
       dispWidth = (double)getWidth() / ((endTime - startTime)-1.0);

    linearDivider = floor((endTime - startTime)/1000);
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
    wOutput1->resize(newBufferSize);
    wOutput2->resize(newBufferSize);
    for(int i=0; i<wOutput1->size(); i++)
    {
        wOutput1->at(i) = 0.0f;
        wOutput2->at(i) = 0.0f;
    }
}

void CalculateDTFT::setLowEndIndex()
{
    low_End_index = round(1.0f * ( newBufferSize / wSampleRate));
}
