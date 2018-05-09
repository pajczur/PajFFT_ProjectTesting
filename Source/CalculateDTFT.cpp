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
    inputData.clear();
    dataIsInUse = false;
    dataIsReadyToFFT = false;
    isForward = true;
}

CalculateDTFT::~CalculateDTFT()
{
}

void CalculateDTFT::setInputData(AudioBuffer<float> &inp)
{
    for(int i=0; i<inp.getNumSamples(); i++)
    {
        if(inputData.size()< (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            inputData.push_back(inp.getSample(0, i));
        }
        else
        {
            if(!inputData.empty())
            {
                dataIsInUse = true;
                
                dataIsReadyToFFT = true;
//                startTimer(1);
                fftCalc();
                
            }
            break;
        }
    }
}

void CalculateDTFT::setInputData(std::vector<float> &inp)
{
    for(int i=0; i<inp.size(); i++)
    {
        if(inputData.size()< (fftType!=2 ? newBufferSize : radix2BuffSize) )
        {
            inputData.push_back(inp[i]);
        }
        else
        {
            if(!inputData.empty())
            {
                dataIsInUse = true;
                dataIsReadyToFFT = true;
                fftCalc();
            }
            break;
        }
    }
}

void CalculateDTFT::setOutputData(std::vector<float> &outp)
{
    
}

//void CalculateDTFT::timerCallback()
//{
//    if(dataIsReadyToFFT)
//    {
//        stopTimer();
//        dataIsReadyToFFT = false;
//        Clock _time;
//        switch (fftType)
//        {
//            case 0:
//                return;
//
//            case 1:
//                mixedRadix_FFT.makeFFT(inputData);
//                if(!isForward)
//                    mixedRadix_IFFT.makeFFT(outCompMixed);
//                break;
//
//            case 2:
//                radix2_FFT.makeFFT(inputData);
//                if(!isForward)
//                    radix2_IFFT.makeFFT(outCompRadix2);
//                break;
//                
//            case 3:
//                regular_DFT.makeDFT(inputData);
//                if(!isForward)
//                    regular_IDFT.makeDFT(outCompDFT);
//                break;
//
//            default:
//                break;
//        }
////        std::cout << _time.secondsElapsed() << std::endl;
//        timeElapsed = _time.secondsElapsed();
//        inputData.clear();
//        dataIsInUse = false;
//    }
//    else
//    {
//        return;
//    }
//}


void CalculateDTFT::fftCalc()
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
                mixedRadix_FFT.makeFFT(inputData);
                if(!isForward)
                    mixedRadix_IFFT.makeFFT(outCompMixed);
                break;
                
            case 2:
                radix2_FFT.makeFFT(inputData);
                if(!isForward)
                    radix2_IFFT.makeFFT(outCompRadix2);
                break;
                
            case 3:
                regular_DFT.makeDFT(inputData);
                if(!isForward)
                    regular_IDFT.makeDFT(outCompDFT);
                break;
                
            default:
                break;
        }
        //        std::cout << _time.secondsElapsed() << std::endl;
        timeElapsed = _time.secondsElapsed();
        inputData.clear();
        dataIsInUse = false;
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
    wOutput->resize(newBufferSize);
    for(int i=0; i<wOutput->size(); i++)
    {
        wOutput->at(i) = 0.0f;
    }
}
