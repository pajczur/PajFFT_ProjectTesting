/*
  ==============================================================================

    LinearDisplay.cpp
    Created: 5 May 2018 7:43:42pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Display_Linear.h"

//==============================================================================
Display_Linear::Display_Linear()
{
    margXLeft = 46.0f-36.0f;
    margXRight = 10.0f;
    margYBot = 20.0f+30.0f;
    margYTop = 22.0f;
    wSampleRateToDisplay = 2.0;

    wZoom.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    addAndMakeVisible(&wZoom);
    wZoom.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    wZoom.addListener(this);
    wZoom.setValue(1.0);

    timeOrWave = 0;
}

Display_Linear::~Display_Linear()
{
}

void Display_Linear::paint (Graphics& g)
{
    _winX = getWidth() - margXLeft - margXRight;
    _winY = getHeight() - margYBot - margYTop;

    g.setColour(Colours::lightblue);
    
    g.drawRect(margXLeft, margYTop, getWidth()-margXRight-margXLeft, _winY);
    
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);
}

void Display_Linear::resized()
{
    wZoom.setBounds(margXLeft-11, getHeight()-33, getWidth()-(margXLeft-11), 30);
}

int Display_Linear::getDisplayHeight()
{
    return getHeight()-margYBot-margYTop;
}

int Display_Linear::getDisplayWidth()
{
    return getWidth()-margXLeft-margXRight;
}

int Display_Linear::getDisplayMargXLeft()
{
    return margXLeft;
}

int Display_Linear::getDisplayMargYTop()
{
    return margYTop;
}

void Display_Linear::setBuffSize(double buffSize)
{
    wSampleRateToDisplay = buffSize;
}

void Display_Linear::sliderValueChanged (Slider *slider)
{
    if(slider == &wZoom)
    {
//          aPlusMinus = (wZoom.getMaxValue() - wZoom.getMinValue()) / 2.0f;
//        
//        if (wZoom.getThumbBeingDragged() == 1 || wZoom.getThumbBeingDragged() == 2)
//              wZoom.setValue(((wZoom.getMaxValue() - wZoom.getMinValue()) / 2.0) + wZoom.getMinValue(), dontSendNotification);
//        else
//        {
//            if(wZoom.getMinValue() <= wZoom.getMinimum() && wZoom.getValue()<midd) {
//                wZoom.setValue(midd, dontSendNotification);
//                return;
//            }
//            else if (wZoom.getMaxValue() >= wZoom.getMaximum() && wZoom.getValue()>midd) {
//                wZoom.setValue(midd, dontSendNotification);
//                return;
//            }
//            else
//                wZoom.setMinAndMaxValues(wZoom.getValue() - aPlusMinus, wZoom.getValue() + aPlusMinus, dontSendNotification);
//        }
//      
//        midd = wZoom.getValue();
//        lowEnd = wZoom.getMinValue();
//        topEnd = wZoom.getMaxValue();
        repaint();
//        graphAnalyser->setZoomLinear(wZoom.getMinValue(), wZoom.getMaxValue());
//        graphAnalyser->waveFormZoom = 1 + (19.0/100.0) * (wZoom.getMaximum()-(wZoom.getMaxValue() - wZoom.getMinValue()));
//        graphAnalyser->waveFormZoomMid = (wZoom.getMaximum()/2.0) - wZoom.getValue();

        graphAnalyser->setZoomLinear(wZoom.getValue());
        graphAnalyser->waveFormZoom = 1 + (19.0/100.0) * wZoom.getValue();
    }
}

void Display_Linear::setReferences(GraphAnalyser &graphToDisplay, AudioPlayer &audioLength)
{
    graphAnalyser = &graphToDisplay;
    aPlayerLength = &audioLength;
}

void Display_Linear::updateZoom()
{
    sliderValueChanged(&wZoom);
}


void Display_Linear::setZoomRangeOscil()
{
    wZoom.setRange(1.0, wSampleRateToDisplay, 0.001);
    wZoom.setSkewFactorFromMidPoint(wSampleRateToDisplay/20.0);
    
//    wZoom.setMinAndMaxValues(0.0, wSampleRateToDisplay);
//    wZoom.setValue( ((wZoom.getMaxValue()-wZoom.getMinValue())/2.0) + wZoom.getMinValue() );
    wZoom.setValue(1.0, sendNotification);
}


void Display_Linear::setZoomRangeTime()
{
    wZoom.setRange(1.0, 2000.0, 0.001);
//    wZoom.setMinAndMaxValues(0.0, 100.0);
//    wZoom.setValue( ((wZoom.getMaxValue()-wZoom.getMinValue())/2.0) + wZoom.getMinValue() );
//    midd = wZoom.getValue();
    wZoom.setValue(1.0, sendNotification);
}
