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
AudioPlayer::AudioPlayer() :   state (Paused), thumbnailCache (5), thumb (30, formatManager, thumbnailCache), isPosChanged(false), isVolChanged(false)
{
    sliderLabelFont.setSizeAndStyle(8.0, 1, 1, 0.5);
    
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (TextButton::buttonColourId, Colours::green);
    playButton.setEnabled (false);
    
    addAndMakeVisible (&pauseButton);
    pauseButton.setButtonText ("Pause");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour (TextButton::buttonColourId, Colours::red);
    pauseButton.setEnabled (false);
    
    addAndMakeVisible(&audioPositionSlider);
    audioPositionSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    audioPositionSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    audioPositionSlider.addListener(this);
    audioPositionSlider.onDragStart = [this] { audioVol.set(0.1); isVolChanged = true; };
    audioPositionSlider.onDragEnd = [this] { audioVol.set(rememberedGain); isVolChanged = true; };
    addAndMakeVisible(&audioPositionLabel);
    audioPositionLabel.setSize(10, 50);
    audioPositionLabel.setFont(sliderLabelFont);
    audioPositionLabel.setText("Transport", dontSendNotification);
    audioPositionLabel.attachToComponent(&audioPositionSlider, false);
    
    addAndMakeVisible(&audioVolumeSlider);
    audioVolumeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    audioVolumeSlider.setRange(0.0, 2.0, 0.00001);
    audioVolumeSlider.setSkewFactorFromMidPoint(0.3);
    audioVolumeSlider.setValue(1.0);
    rememberedGain = audioVolumeSlider.getValue();
    audioVol = rememberedGain;
    audioVolumeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    audioVolumeSlider.addListener(this);
    addAndMakeVisible(&audioVolumeLabel);
    audioVolumeLabel.setSize(10, 50);
    audioVolumeLabel.setFont(sliderLabelFont);
    audioVolumeLabel.setText("Volume", dontSendNotification);
    audioVolumeLabel.attachToComponent(&audioVolumeSlider, false);
    
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
    const int buttonsHeight = 15;
    const int buttonsWidth = getWidth() - 20;
    const int buttonsMargin = 3;
    
    openButton.setBounds (10, 30, getWidth() - 20, buttonsHeight);
    playButton.setBounds (10, openButton.getY()+buttonsHeight+buttonsMargin, buttonsWidth, buttonsHeight);
    pauseButton.setBounds (10, playButton.getY()+buttonsHeight+buttonsMargin, buttonsWidth, buttonsHeight);
    
    audioPositionSlider.setBounds(10, pauseButton.getY()+32, buttonsWidth, 20);
    audioVolumeSlider.setBounds(10, audioPositionSlider.getY()+40, buttonsWidth, 20);
}

void AudioPlayer::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Paused);
    }
}

void AudioPlayer::sliderValueChanged       (Slider *slider)
{
    if (slider == &audioPositionSlider)
    {
        audioPos = audioPositionSlider.getValue();
        isPosChanged = true;
    }
    else if (slider == &audioVolumeSlider)
    {
        rememberedGain = audioVolumeSlider.getValue();
        audioVol = rememberedGain;
        isVolChanged = true;
//        transportSource.setGain(rememberedGain);
    }
}

void AudioPlayer::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch (state)
        {
            case Paused:
                pauseButton.setEnabled (false);
                playButton.setEnabled (true);
//                transportSource.setPosition (0.0);
                break;
                
            case Starting:
                playButton.setEnabled (false);
                transportSource.start();
                break;
                
            case Playing:
                pauseButton.setEnabled (true);
                break;
                
            case Pausing:
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
            transportSource.setGain(audioVolumeSlider.getValue());
            audioPos = transportSource.getCurrentPosition();
            audioPositionSlider.setRange(0.0, transportSource.getLengthInSeconds()/*, 0.00001*/);
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

void AudioPlayer::pauseButtonClicked()
{
    changeState (Pausing);
}


void AudioPlayer::setControlsVisible(bool areVisible)
{
    if(areVisible)
    {  
        openButton.setVisible(true);
        playButton.setVisible(true);
        pauseButton.setVisible(true);
        audioPositionSlider.setVisible(true);
        audioVolumeSlider.setVisible(true);
    }
    else
    {
        openButton.setVisible(false);
        playButton.setVisible(false);
        pauseButton.setVisible(false);
        audioPositionSlider.setVisible(false);
        audioVolumeSlider.setVisible(false);
    }
}
