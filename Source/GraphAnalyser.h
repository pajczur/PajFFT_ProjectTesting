/*
  ==============================================================================

    GraphAnalyser.h
    Created: 9 May 2018 5:56:23pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CalculateDTFT.h"
#include "WavesGen.h"
#include "AudioPlayer.h"

//==============================================================================
/*
*/
class GraphAnalyser    : public Component, public Timer
{
public:
    GraphAnalyser();
    ~GraphAnalyser();
    
    void timerCallback() override;
    
    void setSampleRate(double sample_rate);
    void setNewBufSize(double new_buf_size);
    void setZoomLogar(double lowE, double topE);
    void setZoomLinear(double endTime);
    void setLowEndIndex();
    void setFFT_DataSource(CalculateDTFT &fftData,  WavesGen &oscData, AudioPlayer &audioData);

    void paint (Graphics&) override;
    void resized() override;
    
    void drawLinGraph();
    void drawtimeGraph();
    void drawLogGraph();
    void clearDisplay();
    
    double wDecibels  (double linearMag, int freqIndex);
    double a_weighting(int freqIndex);
    double d_weighting(int freqIndex);

    double deviceBufferSize;
    
    void paintIfNoFileLoaded (Graphics& g);
    void paintIfFileLoaded   (Graphics& g, double &audioPosition);
private:
    Path fftGraph;
public:
    Path wavGraph;
    std::vector<Path> timeGraph;
private:
    CalculateDTFT *dataSource;
    WavesGen      *oscilSource;
    AudioPlayer   *audioSource;
    
    
    double wSampleRate;
    double wBufferSize;
    double nyquist;
    double buffNyquist;
    float maxResolution;

    double dispLogScale;
    float zero_dB;
    double dispWidth;
    float logScaleWidth;
    float low_End;
    float low_End_index;
    float top_End;
    float timeStart;
    float timeEnd;
    float linearDivider;
    float moveG;
    
public:
    int ttt=0;
    int ccc=0;
    int ddd=0;
    bool isFFTon;
    bool isDWeighting;
    bool isFreqAnalyser;
    bool timeTrue_waveFalse;
    bool clearBegin;
    double audioPosition;
    
    bool sourceIsReady;
    
    float waveFormZoom;
    Rectangle<int> chan0Bounds;
    Rectangle<int> chan1Bounds;
    
    AudioThumbnail *thumbnail;
    
    int temppp=0;
    int temppp2=0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphAnalyser)
};
