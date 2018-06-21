/*
  ==============================================================================

    AudioPlayer.cpp
    Created: 17 Apr 2018 7:53:42pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioPlayer.h"

//==============================================================================
AudioPlayer::AudioPlayer() :   state (Stopped), thumbnailCache (5), thumb (30, formatManager, thumbnailCache)
{
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (TextButton::buttonColourId, Colours::green);
    playButton.setEnabled (false);
    
    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Pause");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled (false);
    
    setSize (300, 200);
    
    formatManager.registerBasicFormats();       // [1]
    transportSource.addChangeListener (this);   // [2]
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    g.setColour (Colours::white);
    g.drawText("AUDIO PLAYER", 30, 5, 130, 20, Justification::centredLeft);

}

void AudioPlayer::resized()
{
    openButton.setBounds (10, 10+30, getWidth() - 20, 20);
    playButton.setBounds (10, 40+30, getWidth() - 20, 20);
    stopButton.setBounds (10, 70+30, getWidth() - 20, 20);
}

void AudioPlayer::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
}


void AudioPlayer::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch (state)
        {
            case Stopped:
                stopButton.setEnabled (false);
                playButton.setEnabled (true);
//                transportSource.setPosition (0.0);
                break;
                
            case Starting:
                playButton.setEnabled (false);
                transportSource.start();
                break;
                
            case Playing:
                stopButton.setEnabled (true);
                break;
                
            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void AudioPlayer::openButtonClicked()
{
    FileChooser chooser ("Select a Wave file to play...",
                         File(),
                         "*.wav; *.aif; *.mp3; *.m4a; *.wmv");
    
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        auto* reader = formatManager.createReaderFor (file);
        
        if (reader != nullptr)
        {
            std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            playButton.setEnabled (true);
            readerSource.reset (newSource.release());
            thumb.setSource(new FileInputSource(file));
        }
    }
}

void AudioPlayer::playButtonClicked()
{
    changeState (Starting);
}

void AudioPlayer::stopButtonClicked()
{
    changeState (Stopping);
}


void AudioPlayer::setControlsVisible(bool areVisible)
{
    if(areVisible)
    {
        addAndMakeVisible (&openButton);
        addAndMakeVisible (&playButton);
        addAndMakeVisible (&stopButton);
    }
    else
    {
        openButton.setVisible(false);
        playButton.setVisible(false);
        stopButton.setVisible(false);
    }
}
