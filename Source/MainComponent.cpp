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
    
    addAndMakeVisible(&oscInterface);
    
    addAndMakeVisible(&fftInterface);
    fftInterface.wSettings(calculator_FFT, oscInterface);
    
    addAndMakeVisible(&wAudioPlayer);
    
    
    display_logarithmic.setFFTcalc(calculator_FFT);
    display_logarithmic.setSize(700, 400+30);
    
    display_linear.setFFTcalc(calculator_FFT);
    display_linear.setSize(700, 400+30);
    
    addAndMakeVisible(&calculator_FFT);
    calculator_FFT.setSize(display_logarithmic.getDisplayWidth(), display_logarithmic.getDisplayHeight());
    
    hearFFTinversedSignal = false;
    
    addAndMakeVisible(&selectOscill);
    selectOscill.setToggleState(true, dontSendNotification);
    addAndMakeVisible(&selectPlayer);
    selectOscill.setRadioGroupId(playerOrOscillatorButtons);
    selectOscill.onClick = [this] { updateToggleState(&selectOscill, selectOscill_ID); };
    selectPlayer.setRadioGroupId(playerOrOscillatorButtons);
    selectPlayer.onClick = [this] { updateToggleState(&selectPlayer, selectPlayer_ID); };
    
    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    wSampleRate = sampleRate;
    wBufferSize = samplesPerBlockExpected;
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
            playWaveOscilla(bufferToFill);
        }
        else
        {
            playInversedFFT(bufferToFill);
        }
    }
    


}

void MainComponent::releaseResources()
{
    wAudioPlayer.transportSource.releaseResources();
    
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    if(!fftInterface.wInverseFFT.getToggleState())
    {
        hearFFTinversedSignal = false;
        display_linear.setVisible(false);
        addAndMakeVisible(&display_logarithmic);
    }
    else
    {
        hearFFTinversedSignal = false;
        display_logarithmic.setVisible(false);
        addAndMakeVisible(&display_linear);
    }

}

void MainComponent::resized()
{
    oscInterface.setBounds(10, 10, 130, 410);
    fftInterface.setBounds((getWidth()/2) + 5, 430, 485, 160);
    wAudioPlayer.setBounds(10, 430, 485, 160);
    display_logarithmic.setBounds           (150, 0, 700, 400);
    display_linear.setBounds                (150, 0, 700, 400);
    calculator_FFT.setBounds     (display_logarithmic.getDisplayMargXLeft()+150, display_logarithmic.getDisplayMargYTop(), calculator_FFT.getWidth(), calculator_FFT.getHeight());
    
    selectOscill.setBounds(12, 12, 25, 25);
    selectPlayer.setBounds(12, 432, 25, 25);
}

void MainComponent::fft_defaultSettings()
{
    if(calculator_FFT.isTimerRunning()) calculator_FFT.stopTimer();
    fftInterface.setSampleRate(wSampleRate);
    fftInterface.bufferForFRange = wBufferSize;
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
    
    calculator_FFT.setSampleRate(wSampleRate);
    calculator_FFT.setNewBufSize(wBufferSize);
    calculator_FFT.mixedRadix_FFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealMixed_1, true);

    calculator_FFT.radix2_FFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealRadix2_1, true);
    calculator_FFT.setRadix2BuffSize(wBufferSize);
    
    calculator_FFT.regular_DFT.wSettings(wSampleRate, wBufferSize, calculator_FFT.outRealDFT_1, true);

}

void MainComponent::playWaveOscilla(const AudioSourceChannelInfo& bufferToFill)
{
    oscillator.playWave(*bufferToFill.buffer, bufferToFill.numSamples, bufferToFill.startSample);
    
    if((oscillator.getWaveType() !=0) && calculator_FFT.fftType !=0 && calculator_FFT.isTimerRunning() && !calculator_FFT.dataIsInUse)
    {
        calculator_FFT.setInputData(*bufferToFill.buffer);
    }
}

void MainComponent::playInversedFFT(const AudioSourceChannelInfo& bufferToFill)
{
    oscillator.prepareWave(signalToFFT, bufferToFill.numSamples, bufferToFill.startSample);
    if((oscillator.getWaveType() !=0) && calculator_FFT.fftType !=0 && calculator_FFT.isTimerRunning() && !calculator_FFT.dataIsInUse)
    {
        calculator_FFT.setInputData(signalToFFT);
    }
    
    for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
    {
        float windowing;
        
        if(fftOutputIndex < calculator_FFT.wOutput1->size())
        {
            fftOutputIndex++;
        }
        else
        {
            fftOutputIndex = 1;
        }
        
        windowing = calculator_FFT.wOutput1->at(fftOutputIndex-1);
        
        bufferToFill.buffer->addSample(0, sample, windowing);
    }
}

void MainComponent::playIAudioFile(const AudioSourceChannelInfo& bufferToFill)
{
    wAudioPlayer.transportSource.getNextAudioBlock (bufferToFill);
    if(calculator_FFT.fftType !=0 && calculator_FFT.isTimerRunning() && !calculator_FFT.dataIsInUse)
    {
        calculator_FFT.setInputData(*bufferToFill.buffer);
    }
}


void MainComponent::playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill)
{
    const AudioSourceChannelInfo tempAudioSource(&tempBuff, bufferToFill.startSample, bufferToFill.numSamples);
    
    wAudioPlayer.transportSource.getNextAudioBlock (tempAudioSource);

    
    if(calculator_FFT.fftType !=0 && calculator_FFT.isTimerRunning() && !calculator_FFT.dataIsInUse)
    {
        calculator_FFT.setInputData(*tempAudioSource.buffer);
    }
    
    if(calculator_FFT.dataIsAfterFFT)
    for(int sample = bufferToFill.startSample; sample<bufferToFill.buffer->getNumSamples(); sample++)
    {
        float windowing;

        if(fftOutputIndex < calculator_FFT.wOutput1->size())
        {
            fftOutputIndex++;
            if(fftOutputIndex == calculator_FFT.wOutput1->size())
                calculator_FFT.dataIsAfterFFT = false;
        }
        else
        {
            fftOutputIndex = 1;
            if(oneOfTwo)
                oneOfTwo = false;
            else
                oneOfTwo = true;
        }

//        if(oneOfTwo)
//        {
            windowing = calculator_FFT.wOutput1->at(fftOutputIndex-1);
//        }
//        else
//        {
//            windowing = calculator_FFT.wOutput2->at(fftOutputIndex-1);
//        }

        bufferToFill.buffer->addSample(0, sample, windowing);
    }
}


void MainComponent::updateToggleState(Button* button, int buttonID)
{
    switch (buttonID)
    {
        case 1:
            playerOrOscillat = false;
            break;
            
        case 2:
            playerOrOscillat = true;
            break;

        default:
            return;
    }
}
