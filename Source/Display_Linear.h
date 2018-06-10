/*
  ==============================================================================

    LinearDisplay.h
    Created: 5 May 2018 7:43:42pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GraphAnalyser.h"

//==============================================================================
/*
*/
class Display_Linear    : public Component, public Slider::Listener
{
public:
    Display_Linear();
    ~Display_Linear();
    void sliderValueChanged (Slider *slider) override;

    void paint (Graphics&) override;
    void resized() override;
    int getDisplayHeight();
    int getDisplayWidth();
    int getDisplayMargXLeft();
    int getDisplayMargYTop();
    void setBuffSize(double buffSize);
    void whatToDisplay(GraphAnalyser &graph);
    void updateZoom();
    void setZoomRangeOscil();
    void setZoomRangeTime();


private:
    float topEnd;
    float lowEnd = 1.0f;
    float nyqFr;
    float wSampleRateToDisplay;
    float sampRatMark;
    
    float scale1Hz;
    float scale2Hz;
    float scale3Hz;
    float scale4Hz;
    float scale5Hz;
    float scale10Hz;
    float scale40Hz;
    float scale60Hz;
    float scale440Hz;
    float scale1500Hz;
    float scale2500Hz;
    float scale10000Hz;
    
    float scaleI;
    float scaleII;
    float scaleIII;
    float scaleIV;
    float scaleV;
    float scaleVI;
    float scaleVII;
    float scaleVIII;
    
    
    float margXLeft;
    float margXRight;
    float margYBot;
    float margYTop;
    
public:
    Slider wZoom;
private:
    GraphAnalyser *graphAnalyser;
    
    float _winX;
    float _winY;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Display_Linear)
};
