/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"



//==============================================================================
MainComponent::MainComponent() : fftInterface(this)
{
    
    setSize (1000, 600);
    fPi = 4.0 * atan(1.0);
    fftOutputIndex = 0;
    
    addAndMakeVisible(&graphAnalyser);
    addAndMakeVisible(&oscInterface);
    
    addAndMakeVisible(&fftInterface);
    fftInterface.wSettings(calculator_FFT, oscInterface, graphAnalyser, wAudioPlayer, oscillator);
    
    addAndMakeVisible(&wAudioPlayer);

    display_logarithmic.setFFTcalc(graphAnalyser);
    display_logarithmic.setSize(700, 400+30);
    
    display_linear.setFFTcalc(graphAnalyser);
    display_linear.setSize(700, 400+30);

    graphAnalyser.setSize(display_logarithmic.getDisplayWidth(), display_logarithmic.getDisplayHeight());
    
    hearFFTinversedSignal = false;
    
    addAndMakeVisible(&selectOscill);
    selectOscill.setToggleState(true, dontSendNotification);
    updateToggleState(&selectOscill, selectOscill_ID);
    addAndMakeVisible(&selectPlayer);
    selectOscill.setRadioGroupId(playerOrOscillatorButtons);
    selectOscill.onClick = [this] { updateToggleState(&selectOscill, selectOscill_ID); };
    selectPlayer.setRadioGroupId(playerOrOscillatorButtons);
    selectPlayer.onClick = [this] { updateToggleState(&selectPlayer, selectPlayer_ID); };
    
    fftTimeElapsedLabel.setEditable(false);
    addAndMakeVisible(&fftTimeElapsedLabel);
    fftTimeElapsedLabel.setJustificationType(Justification::centredRight);
    
    fftTimeElapsedInfo.setEditable(false);
    addAndMakeVisible(&fftTimeElapsedInfo);
    fftTimeElapsedInfo.setJustificationType(Justification::centred);
    fftTimeElapsedInfo.setText("Time needs to calc FFT forw or forw AND back, in micro sec", dontSendNotification);
    
    addAndMakeVisible(&pitchShiftON);
    pitchShiftON.setButtonText("Pitch Shift");
    pitchShiftON.setToggleState(false, dontSendNotification);
    updateToggleState(&pitchShiftON, pitchShiftON_ID);
    pitchShiftON.onClick = [this] { updateToggleState(&pitchShiftON, pitchShiftON_ID); };
    
    addAndMakeVisible(&wPitchShift);
    wPitchShift.setSliderStyle(Slider::SliderStyle::LinearVertical);
    wPitchShift.setRange(0.5, 2.0, 0.01);
    wPitchShift.setSkewFactorFromMidPoint(1.0);
    wPitchShift.setValue(1.0);
    wPitchShift.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    wPitchShiftLabel.setText("Pitch Shift", dontSendNotification);
    wPitchShiftLabel.setJustificationType(Justification::centredBottom);
    wPitchShiftLabel.attachToComponent(&wPitchShift, false);
    wPitchShift.addListener(this);
    
    
    startTimer(1000);
    
    // specify the number of input and output channels that we want to open
    
    setAudioChannels (1, 1);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::timerCallback()
{
    if(bufferCounter!=0 && fftTimeElapsed!=0)
    {
        int avarage = fftTimeElapsed/bufferCounter;
        fftTimeElapsedLabel.setText(to_string(avarage), dontSendNotification);
        bufferCounter = 0;
        fftTimeElapsed = 0;
    }
}

void MainComponent::updateToggleState(Button* button, int buttonID)
{
    switch (buttonID)
    {
        case 1: // USE OSCILLATOR
            wAudioPlayer.stopButtonClicked();
            playerOrOscillat = false;
            wAudioPlayer.setControlsVisible(false);
            oscInterface.setControlsVisible(true);
            break;
            
        case 2: // USE AUDIO PLAYER
            oscInterface.wMuteButton.triggerClick();
            playerOrOscillat = true;
            wAudioPlayer.setControlsVisible(true);
            oscInterface.setControlsVisible(false);
            break;
            
        case 3:
            if(pitchShiftON.getToggleState())
            {
                calculator_FFT.mixedRadix_FFT.setWindowing(true);
                
                fftInterface.wInverseFFT.setToggleState(true, dontSendNotification);
                fftInterface.setInverse_fft();
                fftInterface.wInverseFFT.setBounds(85+2000, 125+2000, 80, 30);
                fftInterface.alreadyInversed.setVisible(true);
                
                fftInterface.alreadyWindow.setVisible(true);
                fftInterface.wWindowBut.setVisible(false);
                
                calculator_FFT.isPitchON = true;
            }
            else
            {
                calculator_FFT.mixedRadix_FFT.setWindowing(fftInterface.remembereWinWasClicked);
                fftInterface.alreadyWindow.setVisible(false);

                fftInterface.wInverseFFT.setToggleState(fftInterface.rememberInvWasClicked, dontSendNotification);
                fftInterface.setInverse_fft();

                fftInterface.alreadyInversed.setVisible(false);
                fftInterface.wInverseFFT.setBounds(85, 125, 80, 30);

                if(fftInterface.wInverseFFT.getToggleState())
                    fftInterface.wWindowBut.setVisible(true);
                else
                    fftInterface.wWindowBut.setVisible(false);

                calculator_FFT.isPitchON = false;
            }
            
        default:
            return;
    }
}

void MainComponent::sliderValueChanged (Slider *slider)
{
    if(slider == &wPitchShift)
    {
        calculator_FFT.wPitchShift = wPitchShift.getValue();
    }
}




//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    wSampleRate = sampleRate;
    wBufferSize = samplesPerBlockExpected;
    wAudioPlayer.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    tempBuff.setSize(2, samplesPerBlockExpected);
    fft_defaultSettings();
    std::cout << "buff " << samplesPerBlockExpected << std::endl;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
//    if (wAudioPlayer.readerSource.get() == nullptr)
//    {
//        bufferToFill.clearActiveBufferRegion();
//        return;
//    }
    
    if(oscillator.getWaveType()!=0   ||   (wAudioPlayer.transportSource.isPlaying() && !fftInterface.pauseGetNextAudioBlock))
    {
        if(playerOrOscillat)
        {
            if(!hearFFTinversedSignal)
            {
                playIAudioFile(bufferToFill);
            }
            else
            {
                playInversedFFTAudioFile(bufferToFill);
            }
        }
        else
        {
            if(!hearFFTinversedSignal)
            {
                playWaveGen(bufferToFill);
            }
            else
            {
                playInversedFFTWaveGen(bufferToFill);
            }
        }
        calculateTime();
    }
    else
    {
//        fftOutputIndex=0;
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
}

void MainComponent::releaseResources()
{
    wAudioPlayer.transportSource.releaseResources();
    
//    AudioDeviceManager::AudioDeviceSetup currentAudioSetup;
//    deviceManager.getAudioDeviceSetup (currentAudioSetup);
//    currentAudioSetup.bufferSize = 1024;
//    deviceManager.setAudioDeviceSetup (currentAudioSetup, true);
}



//==============================================================================
void MainComponent::playWaveGen(const AudioSourceChannelInfo& bufferToFill)
{
    oscillator.playWave(*bufferToFill.buffer, bufferToFill.numSamples, bufferToFill.startSample);
    
    if(calculator_FFT.fftType !=0)
    {
        calculator_FFT.setInputData(*bufferToFill.buffer);
    }
    
}

void MainComponent::playInversedFFTWaveGen(const AudioSourceChannelInfo& bufferToFill)
{
    oscillator.prepareWave(signalToFFT, bufferToFill.numSamples, bufferToFill.startSample);
    
    if(calculator_FFT.fftType !=0)
    {
        calculator_FFT.setInputData(signalToFFT);
        
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            float windowing;
            
            if(fftOutputIndex < calculator_FFT.newBufferSize)
            {
                fftOutputIndex++;
            }
            else
            {
                fftOutputIndex = 1;
            }
            
//            std::cout << sample << "    " << fftOutputIndex-1 << std::endl;
            
            windowing = calculator_FFT.outRealMixed[fftOutputIndex-1];
            
            bufferToFill.buffer->addSample(0, sample, windowing);
        }
    }
    else
    {
//        fftOutputIndex=0;
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            bufferToFill.buffer->addSample(0, sample, signalToFFT[sample]);
        }
    }
    
}

void MainComponent::playIAudioFile(const AudioSourceChannelInfo& bufferToFill)
{
    wAudioPlayer.transportSource.getNextAudioBlock (bufferToFill);
    if(calculator_FFT.fftType !=0)
    {
        calculator_FFT.setInputData(*bufferToFill.buffer);
    }
}

void MainComponent::playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill)
{
    const AudioSourceChannelInfo tempAudioSource(&tempBuff, bufferToFill.startSample, bufferToFill.numSamples);
    
    wAudioPlayer.transportSource.getNextAudioBlock (tempAudioSource);

    
    if(calculator_FFT.fftType !=0)
    {
        calculator_FFT.setInputData(*tempAudioSource.buffer);
    }
    
    for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
    {
        float windowing;

        if(fftOutputIndex < calculator_FFT.newBufferSize)
        {
            fftOutputIndex++;
        }
        else
        {
            fftOutputIndex = 1;
        }

        windowing = calculator_FFT.outRealMixed[fftOutputIndex-1];

        bufferToFill.buffer->addSample(0, sample, windowing);
    }
}

void MainComponent::calculateTime()
{
    bufferCounter++;
    fftTimeElapsed += calculator_FFT.timeElapsed;
}



//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    if(!fftInterface.wInverseFFT.getToggleState())
    {
        hearFFTinversedSignal = false;
        display_linear.setVisible(false);
        addAndMakeVisible(&display_logarithmic);
    }
    else
    {
        hearFFTinversedSignal = true; // Set to true if ready playInverseFFT...
        display_logarithmic.setVisible(false);
        addAndMakeVisible(&display_linear);
    }
    
}

void MainComponent::resized()
{
    
    display_logarithmic.setBounds           (150, 0, 700, 400);
    graphAnalyser.setBounds      (display_logarithmic.getDisplayMargXLeft()+150, display_logarithmic.getDisplayMargYTop(), graphAnalyser.getWidth(), graphAnalyser.getHeight());
    display_linear.setBounds                (150, 0, 700, 400);
    oscInterface.setBounds(10, 10, 130, 410);
    fftInterface.setBounds((getWidth()/2) + 5, 430, 485, 160);
    wAudioPlayer.setBounds(10, 430, 485, 160);
    
    fftTimeElapsedInfo.setBounds(getWidth()-130, 10, 120, 60);
    fftTimeElapsedLabel.setBounds(getWidth()-130, 70, 120, 30);
    wPitchShift.setBounds(getWidth()-100, 150, 60, 150);
    pitchShiftON.setBounds(getWidth()-100, 250, 60, 150);
    
    selectOscill.setBounds(12, 12, 25, 25);
    selectPlayer.setBounds(12, 432, 25, 25);
    
}

void MainComponent::fft_defaultSettings()
{
    fftInterface.setSampleRate(wSampleRate);
    fftInterface.rememberedBuffer = wBufferSize;
    
    oscillator.setSampleRate(wSampleRate);
    oscillator.selectWave(0);
    oscillator.setFrequency(440.0);
    oscillator.setAmplitude(1.0);
    oscInterface.settings(oscillator, calculator_FFT, wSampleRate);
    
    display_logarithmic.setNyquist(wSampleRate/2.0);
    display_logarithmic.repaint();
    
    display_linear.setSampRate(wSampleRate);
    display_linear.repaint();
    
    calculator_FFT.setNewBufSize(wBufferSize);
    calculator_FFT.setSampleRate(wSampleRate);
    calculator_FFT.mixedRadix_FFT.wSettings(wSampleRate, wBufferSize);
    
//    calculator_FFT.radix2_FFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealRadix2, true);
    calculator_FFT.setRadix2BuffSize(wBufferSize);
    
//    calculator_FFT.regular_DFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealDFT, true);
    graphAnalyser.setSampleRate(wSampleRate);
    graphAnalyser.setNewBufSize(wBufferSize);
    
    graphAnalyser.setFFT_DataSource(calculator_FFT);
}

