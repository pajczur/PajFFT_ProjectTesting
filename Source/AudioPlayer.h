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
class AudioPlayer    : public Component, public ChangeListener
{
public:
    AudioPlayer();
    ~AudioPlayer();

    void paint (Graphics&) override;
    void resized() override;
    
    void setControlsVisible(bool areVisible);
    
    void changeListenerCallback (ChangeBroadcaster* source) override;

private:
    
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    
    void changeState (TransportState newState);
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    
    
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    
    AudioFormatManager formatManager;
    TransportState state;
public:
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayer)
};
