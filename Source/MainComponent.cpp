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
    selectOscill.onClick = [this] { updateToggleState(&selectOscill, selectOscill_ID); };
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
    fftTimeElapsedInfo.setText("Time needs to calc FFT forw or forw AND back, in micro sec", dontSendNotification);

  
    // == Choose Frequency graph == //
    addAndMakeVisible(&freqDisp);
    freqDisp.setRadioGroupId(selectorFreqTimeButton);
    freqDisp.setAlwaysOnTop(true);
    freqDisp.setButtonText("Freq");
    freqDisp.setToggleState(false, dontSendNotification);
    freqDisp.onClick = [this] { updateToggleState(&freqDisp, freqDisp_ID); };
  
    // == D-weighting correction == //
    addAndMakeVisible(&d_weightingDisp);
    d_weightingDisp.setAlwaysOnTop(true);
    d_weightingDisp.setButtonText("D-weighting");
    d_weightingDisp.setToggleState(false, dontSendNotification);
    d_weightingDisp.onClick = [this] { updateToggleState(&d_weightingDisp, d_weightingDisp_ID); };
    
    // == Choose Time (wave) graph == //
    addAndMakeVisible(&timeDisp);
    timeDisp.setRadioGroupId(selectorFreqTimeButton);
    timeDisp.setAlwaysOnTop(true);
    timeDisp.setButtonText("Time");
    timeDisp.setToggleState(true, dontSendNotification);
    timeDisp.onClick = [this] { updateToggleState(&timeDisp, timeDisp_ID); };
    updateToggleState(&timeDisp, timeDisp_ID);
  
    // == set up all references == //
    oscInterface.setReferences(oscillator, calculator_FFT, graphAnalyser);
    pitchShiftGui.setReferences(calculator_FFT, fftInterface);
    fftInterface.setReferences(calculator_FFT, oscInterface, graphAnalyser, wAudioPlayer, oscillator);
    display_logarithmic.whatToDisplay(graphAnalyser);
    display_linear.whatToDisplay(graphAnalyser);
    graphAnalyser.setFFT_DataSource(calculator_FFT, oscillator, wAudioPlayer);
    
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

            
        case 4:
            graphAnalyser.isFreqAnalyser = true;
            d_weightingDisp.setVisible(true);
            display_linear.setVisible(false);
            display_logarithmic.setVisible(true);
            graphAnalyser.setBounds(display_logarithmic.getDisplayMargXLeft()+151, display_logarithmic.getDisplayMargYTop()+10, 644-2, 338);
            graphAnalyser.setVisible(true);
            break;
            
        case 5:
            graphAnalyser.isFreqAnalyser = false;
            d_weightingDisp.setVisible(false);
            display_logarithmic.setVisible(false);
            display_linear.setVisible(true);
            graphAnalyser.setBounds(display_linear.getDisplayMargXLeft()+151, display_linear.getDisplayMargYTop()+10, 644+36-2, 338);
            graphAnalyser.setVisible(true);
            break;
            
        case 6:
            if(d_weightingDisp.getToggleState())
            {
                graphAnalyser.isDWeighting = true;
            }
            else
                graphAnalyser.isDWeighting = false;
            break;
            
        default:
            return;
    }
}

//void MainComponent::sliderValueChanged (Slider *slider)
//{
//    if(slider == &wPitchShift)
//    {
//        calculator_FFT.wPitchShift = wPitchShift.getValue();
//    }
//}




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
        calculator_FFT.fftCalculator(*bufferToFill.buffer);
    }
    else
    {
        calculator_FFT.getInputData(*bufferToFill.buffer);
    }
}

void MainComponent::playInversedFFTWaveGen(const AudioSourceChannelInfo& bufferToFill)
{
    if(calculator_FFT.fftType !=0)
    {
        oscillator.prepareWave(signalToFFT, bufferToFill.numSamples, bufferToFill.startSample);
        calculator_FFT.fftCalculator(signalToFFT);
        
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            float windowing;
            
//            if(fftOutputIndex < calculator_FFT.newBufferSize)
//            {
//                fftOutputIndex++;
//            }
//            else
//            {
//                fftOutputIndex = 1;
//            }
//            windowing = calculator_FFT.outRealMixed[fftOutputIndex-1];
//            windowing = calculator_FFT.wOutput[fftOutputIndex-1];
            
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
    if(calculator_FFT.fftType !=0)
    {
        calculator_FFT.fftCalculator(*bufferToFill.buffer);
    }
    else
    {
        calculator_FFT.getInputData(*bufferToFill.buffer);
        if(!graphAnalyser.isTimerRunning())
        {
            graphAnalyser.startTimer(40);
        }
    }
}

void MainComponent::playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill)
{

    
    if(calculator_FFT.fftType !=0)
    {
        const AudioSourceChannelInfo tempAudioSource(&tempBuff, bufferToFill.startSample, bufferToFill.numSamples);
        wAudioPlayer.transportSource.getNextAudioBlock (tempAudioSource);
        
        calculator_FFT.fftCalculator(*tempAudioSource.buffer);
        
        for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
        {
            float windowing;
            
//            if(fftOutputIndex < calculator_FFT.newBufferSize)
//            {
//                fftOutputIndex++;
//            }
//            else
//            {
//                fftOutputIndex = 1;
//            }
//            windowing = calculator_FFT.outRealMixed[fftOutputIndex-1];
            
            windowing = calculator_FFT.wOutput[sample];
            bufferToFill.buffer->addSample(0, sample, windowing);
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
    
    if(!fftInterface.wInverseFFT.getToggleState())
    {
        hearFFTinversedSignal = false;
    }
    else
    {
        hearFFTinversedSignal = true; // Set to true if ready playInverseFFT...
    }
    g.setColour (Colours::red);
    
    if(freqDisp.getToggleState() && calculator_FFT.fftType==0 && oscillator.getWaveType()==0 && (wAudioPlayer.state != wAudioPlayer.Playing))
        g.drawText("Choose FFT type and sound source", 10, 50, getWidth()-20, 50, Justification::centredTop);
    if(freqDisp.getToggleState() && calculator_FFT.fftType==0 && (oscillator.getWaveType()!=0 || (wAudioPlayer.state == wAudioPlayer.Playing)))
        g.drawText("Choose FFT type", 10, 50, getWidth()-20, 50, Justification::centredTop);
    if(freqDisp.getToggleState() && calculator_FFT.fftType!=0 && oscillator.getWaveType()==0 && (wAudioPlayer.state == wAudioPlayer.Playing))
        g.drawText("Choose sound source", 10, 50, getWidth()-20, 50, Justification::centredTop);
}

void MainComponent::resized()
{
    
    display_logarithmic.setBounds           (150, 10, 700, 410);
    display_linear.setBounds                (150, 10, 700, 410);
    pitchShiftGui.setBounds(860, 100, 130, 320);
    oscInterface.setBounds(10, 10, 130, 410);
    fftInterface.setBounds((getWidth()/2) + 5, 430, 485, 160);
    wAudioPlayer.setBounds(10, 430, 485, 160);
    
    fftTimeElapsedInfo.setBounds(getWidth()-130, 10, 110, 60);
    fftTimeElapsedLabel.setBounds(getWidth()-130, 60, 110, 30);

    selectOscill.setBounds(12, 12, 25, 25);
    selectPlayer.setBounds(12, 432, 25, 25);
  
    freqDisp.setBounds(355, 12, 80, 17);
    timeDisp.setBounds(440, 12, 80, 17);
    d_weightingDisp.setBounds(525, 12, 110, 17);
}

void MainComponent::fft_defaultSettings()
{
    oscillator.setSampleRate(wSampleRate);
    oscillator.selectWave(0);
    oscillator.setFrequency(220.0);
    oscillator.setAmplitude(0.5);
    oscInterface.setSampleRate(wSampleRate);

    
    fftInterface.setSampleRate(wSampleRate);
    fftInterface.setBufferSize(deviceBufferSize);
    fftInterface.rememberedBuffer = deviceBufferSize;
    
    
    display_logarithmic.setNyquist(wSampleRate/2.0);
    display_logarithmic.repaint();
    
    display_linear.setSampRate(wSampleRate);
    display_linear.repaint();
    
    calculator_FFT.defineDeviceBuffSize((long)deviceBufferSize);
    calculator_FFT.setNewBufSize(deviceBufferSize);
    calculator_FFT.setSampleRate(wSampleRate);
    calculator_FFT.mixedRadix_FFT.wSettings(wSampleRate, deviceBufferSize);
    
//    calculator_FFT.radix2_FFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealRadix2, true);
    calculator_FFT.setRadix2BuffSize(deviceBufferSize);
    
//    calculator_FFT.regular_DFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealDFT, true);
    
    calculator_FFT.resetOutputData();
    
    graphAnalyser.setSampleRate(wSampleRate);
    graphAnalyser.setNewBufSize(deviceBufferSize);
    
    
}

