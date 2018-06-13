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
    wSampleRateToDisplay = 1.0;

    wZoom.setSliderStyle(Slider::SliderStyle::ThreeValueHorizontal);
    addAndMakeVisible(&wZoom);
    wZoom.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    wZoom.addListener(this);
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
      	float diff = (wZoom.getMaxValue() - wZoom.getMinValue()) / 2.0f;
      
		    if (wZoom.getThumbBeingDragged() == 1 || wZoom.getThumbBeingDragged() == 2)
			      wZoom.setValue(((wZoom.getMaxValue() - wZoom.getMinValue()) / 2.0) + wZoom.getMinValue(), dontSendNotification);
		    else
		      	wZoom.setMinAndMaxValues(wZoom.getValue() - diff, wZoom.getValue() + diff, dontSendNotification);
      
        //wZoom.setValue(((wZoom.getMaxValue()-wZoom.getMinValue())/2.0)+wZoom.getMinValue());

        lowEnd = wZoom.getMinValue();
        topEnd = wZoom.getMaxValue();
        repaint();
        graphAnalyser->setZoomLinear(wZoom.getMinValue(), wZoom.getMaxValue());
        graphAnalyser->waveFormZoom = wZoom.getMinValue();
        
    }
}

void Display_Linear::whatToDisplay(GraphAnalyser &graph)
{
    graphAnalyser = &graph;
}

void Display_Linear::updateZoom()
{
    sliderValueChanged(&wZoom);
}


void Display_Linear::setZoomRangeOscil()
{
    wZoom.setRange(0.0, wSampleRateToDisplay, 0.001);
    
    wZoom.setMinAndMaxValues(0.0, wSampleRateToDisplay);
    wZoom.setValue( ((wZoom.getMaxValue()-wZoom.getMinValue())/2.0) + wZoom.getMinValue() );
}


void Display_Linear::setZoomRangeTime()
{
    wZoom.setRange(0.0, 100.0, 0.001);
    
    wZoom.setMinAndMaxValues(0.0, 100.0);
    wZoom.setValue( ((wZoom.getMaxValue()-wZoom.getMinValue())/2.0) + wZoom.getMinValue() );
}
