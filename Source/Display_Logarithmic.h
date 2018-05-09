/*
  ==============================================================================

    DisplayWindow.h
    Created: 18 Apr 2018 10:20:17pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GraphAnalyser.h"

//==============================================================================
/*
*/
class Display_Logarithmic    : public Component, public Slider::Listener
{
public:
    Display_Logarithmic();
    ~Display_Logarithmic();
    void sliderValueChanged (Slider *slider) override;

    void paint (Graphics&) override;
    void resized() override;
    int getDisplayHeight();
    int getDisplayWidth();
    int getDisplayMargXLeft();
    int getDisplayMargYTop();
    void setNyquist(double nyquistFreq);
    void setFFTcalc(GraphAnalyser &graph);
    
//    void set

private:
    float wNyquist = 1.0;
    float topEnd;
    float lowEnd = 1.0f;
    float nyqFr;
    float sampRat;
    float sampRatMark;
    
    float scale1Hz;
    float scale2Hz;
    float scale3Hz;
    float scale4Hz;
    float scale5Hz;
    float scale6Hz;
    float scale7Hz;
    float scale8Hz;
    float scale10Hz;
    float scale20Hz;
    float scale30Hz;
    float scale40Hz;
    float scale50Hz;
    float scale60Hz;
    float scale70Hz;
    float scale80Hz;
    float scale100Hz;
    float scale200Hz;
    float scale300Hz;
    float scale400Hz;
    float scale500Hz;
    float scale600Hz;
    float scale700Hz;
    float scale800Hz;
    float scale1000Hz;
    float scale2000Hz;
    float scale3000Hz;
    float scale4000Hz;
    float scale5000Hz;
    float scale6000Hz;
    float scale7000Hz;
    float scale8000Hz;
    
    float scale10000Hz;
    float scale20000Hz;
    float scaleNyquist;
    float scaleSampRat;
    
    
    float margXLeft;
    float margXRight;
    float margYBot;
    float margYTop;
    
    Slider wZoom;
    GraphAnalyser *graphAnalyser;
    
    float _winX;
    float _winY;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Display_Logarithmic)
};
