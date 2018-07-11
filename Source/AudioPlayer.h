/*
  ==============================================================================

    AudioPlayer.h
    Created: 17 Apr 2018 7:53:42pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

class AudioPlayer    : public Component, public ChangeListener, public Slider::Listener
{
public:
    AudioPlayer();
    ~AudioPlayer();

    void paint (Graphics&) override;
    void resized() override;
    
    void setControlsVisible(bool areVisible);
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void sliderValueChanged (Slider *slider) override;

public:
    
    enum TransportState
    {
        Paused,
        Starting,
        Playing,
        Pausing
    };
    
public:
    void changeState (TransportState newState);
    
    void openButtonClicked();
    void playButtonClicked();
    void pauseButtonClicked();
    
private:
    TextButton openButton;
    TextButton playButton;
    TextButton pauseButton;
public:
    Slider audioVolumeSlider;
    Label  audioVolumeLabel;
    Slider audioPositionSlider;
    Atomic<double> audioPos, audioVol;
private:
    Label  audioPositionLabel;
    Font sliderLabelFont;
    double rememberedGain;
    
    
    AudioFormatManager formatManager;
public:
    TransportState state;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    
    AudioThumbnailCache thumbnailCache;     
    AudioThumbnail thumb;
    Atomic<bool> isPosChanged, isVolChanged;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayer)
};
