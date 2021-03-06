/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"



//==============================================================================
MainComponent::MainComponent()
        : adsc(deviceManager, 0, 0, 0, 0, false, false, false, false),
          fftInterface(this),
          audioPosition(wAudioPlayer.audioPos), audioVolume(wAudioPlayer.audioVol),
          isPositionChanged(wAudioPlayer.isPosChanged), isVolumeChanged(wAudioPlayer.isVolChanged)
{
    addAndMakeVisible(adsc);
    adsc.setAlwaysOnTop(true);
    
    
    
    setSize (1000, 600);
    fPi = 4.0 * atan(1.0);
    fftOutputIndex = 0;
    hearFFTinversedSignal = false;
    
    // == Set up GUI == //
    addAndMakeVisible(&oscInterface);
    addAndMakeVisible(&wAudioPlayer);
    addAndMakeVisible(&fftInterface);
    addAndMakeVisible(&pitchShiftGui);
    
    addAndMakeVisible(&display_logarithmic);
    addAndMakeVisible(&display_linear);
    addAndMakeVisible(&graphAnalyser);
    
    // == Choose Oscillator or Player == //
    addAndMakeVisible(&selectOscill);
    selectOscill.setRadioGroupId(playerOrOscillatorButtons);
    selectOscill.onClick = [this] { updateToggleState(&selectOscill, ButtonID::selectOscill_ID); };
    selectOscill.setToggleState(true, dontSendNotification);
    updateToggleState(&selectOscill, selectOscill_ID);
    
    addAndMakeVisible(&selectPlayer);
    selectPlayer.setRadioGroupId(playerOrOscillatorButtons);
    selectPlayer.onClick = [this] { updateToggleState(&selectPlayer, selectPlayer_ID); };
    
    
    // == Display Calculator efficiency == //
    fftTimeElapsedLabel.setEditable(false);
    addAndMakeVisible(&fftTimeElapsedLabel);
    fftTimeElapsedLabel.setJustificationType(Justification::centredTop);
    
    fftTimeElapsedInfo.setEditable(false);
    addAndMakeVisible(&fftTimeElapsedInfo);
    fftTimeElapsedInfo.setJustificationType(Justification::centredTop);
    fftTimeElapsedInfo.setText("Time needs to calc FFT forw or forw and back [micro sec]", dontSendNotification);
    fftTimeElapsedLabel.setColour(Label::textColourId, Colours::red);
  
    // == Choose Frequency graph == //
    addAndMakeVisible(&freqDisp);
    freqDisp.setRadioGroupId(selectorFreqTimeButton);
    freqDisp.setAlwaysOnTop(true);
    freqDisp.setButtonText("Freq");
    freqDisp.setToggleState(true, dontSendNotification);
    freqDisp.onClick = [this] { updateToggleState(&freqDisp, freqDisp_ID); };
  
    // == D-weighting correction == //
    addAndMakeVisible(&d_weightingDisp);
    d_weightingDisp.setAlwaysOnTop(true);
    d_weightingDisp.setButtonText("D-weighting");
    d_weightingDisp.setToggleState(false, dontSendNotification);
    d_weightingDisp.onClick = [this] { updateToggleState(&d_weightingDisp, d_weightingDisp_ID); };
    
    // == Choose Time graph == //
    addAndMakeVisible(&timeDisp);
    timeDisp.setRadioGroupId(selectorFreqTimeButton);
    timeDisp.setAlwaysOnTop(true);
    timeDisp.setButtonText("Time");
    timeDisp.setToggleState(false, dontSendNotification);
    timeDisp.onClick = [this] { updateToggleState(&timeDisp, timeDisp_ID); };
    timeDisp.setVisible(false);
    
    // == Choose Wave graph == //
    addAndMakeVisible(&waveDisp);
    waveDisp.setRadioGroupId(selectorFreqTimeButton);
    waveDisp.setAlwaysOnTop(true);
    waveDisp.setButtonText("Wave");
    waveDisp.setToggleState(false, dontSendNotification);
    waveDisp.onClick = [this] { updateToggleState(&waveDisp, waveDisp_ID); };
    
    
    // == Turn OFF graph == //
    addAndMakeVisible(&graphOff);
    graphOff.setRadioGroupId(selectorFreqTimeButton);
    graphOff.setAlwaysOnTop(true);
    graphOff.setButtonText("OFF");
    graphOff.setToggleState(false, dontSendNotification);
    graphOff.onClick = [this] { updateToggleState(&graphOff, ButtonID::graphOff_ID); };

  
    // == set up all references == //
    oscInterface.setReferences(oscillator, calculator_FFT, graphAnalyser);
    pitchShiftGui.setReferences(calculator_FFT, fftInterface);
    fftInterface.setReferences(calculator_FFT, oscInterface, graphAnalyser, wAudioPlayer, oscillator, display_linear);
    display_logarithmic.whatToDisplay(graphAnalyser);
    display_linear.setReferences(graphAnalyser, wAudioPlayer);
    graphAnalyser.setFFT_DataSource(calculator_FFT, oscillator, wAudioPlayer);
    
    updateToggleState(&freqDisp, freqDisp_ID);

    startTimer(1000);
    
    setAudioChannels (2, 2);
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

void MainComponent::updateToggleState(Button* button, ButtonID buttonID)
{
    if(button->getToggleState()  || button == &d_weightingDisp)
    {
        switch (buttonID)
        {
            case graphOff_ID:
                graphAnalyser.isFreqAnalyser = false;
                graphAnalyser.wavGraph.clear();
                d_weightingDisp.setVisible(false);
                display_logarithmic.setVisible(false); // CO TO JEST ???
                display_linear.setVisible(false);
                graphAnalyser.setVisible(false);
                graphAnalyser.stopTimer();
                oscInterface.isGraphOn = false;
                fftInterface.isGraphON = false;
                break;
                
            case selectOscill_ID: // USE OSCILLATOR
                wAudioPlayer.pauseButtonClicked();
                playerOrOscillat = false;
                wAudioPlayer.setControlsVisible(false);
                oscInterface.setControlsVisible(true);
                timeDisp.setVisible(false);
                if(timeDisp.getToggleState()) {
                    freqDisp.setToggleState(true, dontSendNotification);
                    updateToggleState(&freqDisp, freqDisp_ID);
                }
                break;
                
            case selectPlayer_ID: // USE AUDIO PLAYER
                oscInterface.wMuteButton.triggerClick();
                playerOrOscillat = true;
                wAudioPlayer.setControlsVisible(true);
                oscInterface.setControlsVisible(false);
                timeDisp.setVisible(true);
                break;

                
            case freqDisp_ID: // freq graph
                graphAnalyser.isFreqAnalyser = true;
                d_weightingDisp.setVisible(true);
                display_linear.setVisible(false);
                display_logarithmic.setVisible(true);
                graphAnalyser.setBounds(display_logarithmic.getDisplayMargXLeft()+151, display_logarithmic.getDisplayMargYTop()+10, 644-2, 338);
                graphAnalyser.setVisible(true);
                display_linear.timeOrWave = 0;
                fftInterface.isGraphON = true;
                if(calculator_FFT.fftIsReady.get()) {
                    oscInterface.isGraphOn = true;
                    if(!graphAnalyser.isTimerRunning())
                        graphAnalyser.startTimer(40);
                }
                else {
                    oscInterface.isGraphOn = false;
                    graphAnalyser.stopTimer();
                }
                break;
                
            case timeDisp_ID: // time graph
                graphAnalyser.isFreqAnalyser = false;
                graphAnalyser.timeTrue_waveFalse = true;
                graphAnalyser.wavGraph.clear();
                d_weightingDisp.setVisible(false);
                display_linear.setZoomRangeTime();
                display_logarithmic.setVisible(false);
                display_linear.wZoom.setValue(0.0f);
                display_linear.timeOrWave = 1;
                
                display_linear.sliderValueChanged(&display_linear.wZoom);
                display_linear.setVisible(true);
                graphAnalyser.setBounds(display_linear.getDisplayMargXLeft()+151, display_linear.getDisplayMargYTop()+10, 644+36-2, 338);
                graphAnalyser.setVisible(true);
                display_linear.updateZoom();
                oscInterface.isGraphOn = true;
                fftInterface.isGraphON = true;
                if(!graphAnalyser.isTimerRunning())
                    graphAnalyser.startTimer(40);
                break;
                
            case waveDisp_ID: // wave graph
                graphAnalyser.isFreqAnalyser = false;
                graphAnalyser.timeTrue_waveFalse = false;
                graphAnalyser.wavGraph.clear();
                d_weightingDisp.setVisible(false);
                display_linear.setZoomRangeOscil();
                display_logarithmic.setVisible(false);
                display_linear.wZoom.setValue(0.0f);
                display_linear.timeOrWave = 2;
                
                display_linear.sliderValueChanged(&display_linear.wZoom);
                display_linear.setVisible(true);
                graphAnalyser.setBounds(display_linear.getDisplayMargXLeft()+151, display_linear.getDisplayMargYTop()+10, 644+36-2, 338);
                graphAnalyser.setVisible(true);
                display_linear.updateZoom();
                oscInterface.isGraphOn = true;
                fftInterface.isGraphON = true;
                if(!graphAnalyser.isTimerRunning())
                    graphAnalyser.startTimer(40);
                break;
                
            case d_weightingDisp_ID:
                if(d_weightingDisp.getToggleState()) {
                    graphAnalyser.isDWeighting = true;
                }
                else {
                    graphAnalyser.isDWeighting = false;
                }
                break;
                
            default:
                return;
        }
    }
}




//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    wSampleRate = sampleRate;
    deviceBufferSize = samplesPerBlockExpected;
    wAudioPlayer.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    tempBuff.setSize(2, samplesPerBlockExpected);
    fft_defaultSettings();
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
            if(isPositionChanged.get())
            {
                 wAudioPlayer.transportSource.setPosition(audioPosition.get());
                isPositionChanged = false;
            }
            if(isVolumeChanged.get())
            {
                wAudioPlayer.transportSource.setGain(audioVolume.get());
                isVolumeChanged = false;
            }
            
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
        fftOutputIndex=0;
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if(timeDisp.getToggleState())
        calculator_FFT.dataIsReadyToGraph = true;
    
}

void MainComponent::releaseResources()
{
    wAudioPlayer.transportSource.releaseResources();
}



//==============================================================================
void MainComponent::playWaveGen(const AudioSourceChannelInfo& bufferToFill)
{
    oscillator.playWave(*bufferToFill.buffer, bufferToFill.numSamples, bufferToFill.startSample);
    
    if(calculator_FFT.fftIsReady.get())
    {
        calculator_FFT.fftCalculator(*bufferToFill.buffer);
    }
    else
    {
        calculator_FFT.getInputData(*bufferToFill.buffer);
    }
}

void MainComponent::playInversedFFTWaveGen(const AudioSourceChannelInfo& bufferToFill)
{
    if(calculator_FFT.fftIsReady.get())
    {
        oscillator.prepareWave(signalToFFT, bufferToFill.numSamples, bufferToFill.startSample);
        calculator_FFT.fftCalculator(signalToFFT);
        
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            float windowing;
            
            windowing = calculator_FFT.wOutput[sample];
            
            bufferToFill.buffer->addSample(0, sample, windowing);
        }
    }
    else
    {
        fftOutputIndex=0;
        oscillator.playWave(*bufferToFill.buffer, bufferToFill.numSamples, bufferToFill.startSample);
    }
}

void MainComponent::playIAudioFile(const AudioSourceChannelInfo& bufferToFill)
{
    wAudioPlayer.transportSource.getNextAudioBlock (bufferToFill);
    if(calculator_FFT.fftIsReady.get())
    {
        calculator_FFT.fftCalculator(*bufferToFill.buffer);
    }
    else
    {
        calculator_FFT.getInputData(*bufferToFill.buffer);
        if(!graphAnalyser.isTimerRunning()    &&   !graphOff.getToggleState())
        {
            graphAnalyser.startTimer(40);
        }
    }
}

void MainComponent::playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill)
{

    
    if(calculator_FFT.fftIsReady.get())
    {
        const AudioSourceChannelInfo tempAudioSource(&tempBuff, bufferToFill.startSample, bufferToFill.numSamples);
        wAudioPlayer.transportSource.getNextAudioBlock (tempAudioSource);
        
        calculator_FFT.fftCalculator(*tempAudioSource.buffer);
        
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            float windowing;
            
            windowing = calculator_FFT.wOutput[sample];
            bufferToFill.buffer->addSample(0, sample, windowing);
            bufferToFill.buffer->addSample(1, sample, windowing);
        }
    }
    else
    {
        fftOutputIndex=0;
        wAudioPlayer.transportSource.getNextAudioBlock (bufferToFill);
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
    g.setColour (Colours::grey);
    g.drawRect(getWidth()-140, 10, 130, 80);
    g.drawRect(30 + 485-140+ 485-215, 430, 130+215, 160);
    
    if(!fftInterface.wInverseFFT.getToggleState() || fftInterface.whatIsChanged_ID==0)
    {
        hearFFTinversedSignal = false;
    }
    else
    {
        hearFFTinversedSignal = true; // Set to true if ready playInverseFFT...
    }
    
    if((wAudioPlayer.audioPositionSlider.isMouseOverOrDragging() ||  wAudioPlayer.audioVolumeSlider.isMouseOverOrDragging()) && !graphAnalyser.isTimerRunning())
    {
        graphAnalyser.startTimer(40);
    }
    
    if(fftInterface.wInverseFFT.getToggleState())
        pitchShiftGui.setControlsVisible(true);
    else
        pitchShiftGui.setControlsVisible(false);
    g.setColour (Colours::red);
    
    if(freqDisp.getToggleState() && calculator_FFT.fftType==0 && oscillator.getWaveType()==0 && (wAudioPlayer.state != wAudioPlayer.Playing))
        g.drawText("Choose FFT type and sound source", 10, 50, getWidth()-20, 50, Justification::centredTop);
    if(freqDisp.getToggleState() && calculator_FFT.fftType==0 && (oscillator.getWaveType()!=0 || (wAudioPlayer.state == wAudioPlayer.Playing)))
        g.drawText("Choose FFT type", 10, 50, getWidth()-20, 50, Justification::centredTop);
    if((( (freqDisp.getToggleState() || waveDisp.getToggleState() ) && calculator_FFT.fftType!=0) || (timeDisp.getToggleState() || waveDisp.getToggleState())) && oscillator.getWaveType()==0 && (wAudioPlayer.state != wAudioPlayer.Playing))
        g.drawText("Choose sound source", 10, 50, getWidth()-20, 50, Justification::centredTop);
    
    if(wAudioPlayer.state == wAudioPlayer.Playing   &&   !graphAnalyser.isTimerRunning()   &&   !graphOff.getToggleState())
        graphAnalyser.startTimer(40);
}

void MainComponent::resized()
{
    
    display_logarithmic.setBounds           (150, 10, 700, 410);
    display_linear.setBounds                (150, 10, 700, 410);
    pitchShiftGui.setBounds(860, 100, 130, 320);
    oscInterface.setBounds(10, 10, 130, 410);
    wAudioPlayer.setBounds(10, 430, 130, 160);
    fftInterface.setBounds((getWidth()/2) + 5-140-215, 430, 485, 160);
    adsc.setBounds(660, 420, 330, 160);
    
    fftTimeElapsedInfo.setBounds(getWidth()-135, 15, 120, 60);
    fftTimeElapsedLabel.setBounds(getWidth()-135, 65, 120, 30);

    selectOscill.setBounds(12, 12, 25, 25);
    selectPlayer.setBounds(12, 432, 25, 25);
  
    freqDisp.setBounds(440, 12, 50, 17);
    waveDisp.setBounds(500, 12, 50, 17);
    timeDisp.setBounds(560, 12, 50, 17);
    graphOff.setBounds(380, 12, 50, 17);
    d_weightingDisp.setBounds(750, 12, 110, 17);
}

void MainComponent::fft_defaultSettings()
{
    oscillator.setSampleRate(wSampleRate);
    oscillator.selectWave(0);
    oscillator.setFrequency(220.0);
    oscillator.setAmplitude(0.5);
    oscInterface.setSampleRate(wSampleRate);
    oscInterface.updateToggleState(&oscInterface.wMuteButton, oscInterface.mutIdentifier);
    wAudioPlayer.pauseButtonClicked();

    
    fftInterface.setSampleRate(wSampleRate);
    fftInterface.setBufferSize(deviceBufferSize);
    fftInterface.rememberedBuffer = deviceBufferSize;
    fftInterface.updateToggleState(&fftInterface.turnOFF, fftInterface.turnOFF_ID);
    
    
    display_logarithmic.setNyquist(wSampleRate/2.0);
    display_logarithmic.repaint();
    
    display_linear.setBuffSize(deviceBufferSize);
    display_linear.repaint();
    
    calculator_FFT.defineDeviceBuffSize((long)deviceBufferSize);
    calculator_FFT.setSampleRate(wSampleRate, (long)fftInterface.setWindowOverLap.getValue());
    calculator_FFT.setNewBufSize(deviceBufferSize);
    calculator_FFT.mixedRadix_FFT.wSettings(wSampleRate, deviceBufferSize);
    
    calculator_FFT.radix2_FFT.wSettings(wSampleRate, deviceBufferSize);
    
    graphAnalyser.setSize(642, 338);
    graphAnalyser.setSampleRate(wSampleRate);
    graphAnalyser.setNewBufSize(deviceBufferSize);
    graphAnalyser.deviceBufferSize = deviceBufferSize;
    if(timeDisp.getToggleState()) display_linear.setZoomRangeTime();
    if(waveDisp.getToggleState()) display_linear.setZoomRangeOscil();
}

