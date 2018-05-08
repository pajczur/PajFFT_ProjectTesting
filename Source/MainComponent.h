/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscInterface.h"
#include "FFTInterface.h"
#include "AudioPlayer.h"
#include "WavesGen.h"
#include "Display_Logarithmic.h"
#include "Display_Linear.h"
#include "CalculateDTFT.h"
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void fft_defaultSettings();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void playWaveOscilla(const AudioSourceChannelInfo& bufferToFill);
    void playInversedFFT(const AudioSourceChannelInfo& bufferToFill);
    
    void playIAudioFile(const AudioSourceChannelInfo& bufferToFill);
    void playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill);
    
    void updateToggleState(Button* button, int buttonID);

    Display_Logarithmic display_logarithmic;
    Display_Linear      display_linear;
private:
    //==============================================================================
    // Your private member variables go here...
    CalculateDTFT calculator_FFT;
    AudioBuffer<float> tempBuff;
    

    FFTInterface fftInterface;
    OscInterface oscInterface;
    WavesGen oscillator;
    AudioPlayer wAudioPlayer;
    
    double wSampleRate;
    double wBufferSize;
    
    ToggleButton selectFreqDisp;
    ToggleButton selectTimeDisp;
    const int freqDisp_ID=1;
    const int timeDisp_ID=2;
    
    ToggleButton selectOscill;
    ToggleButton selectPlayer;
    const int selectOscill_ID=1;
    const int selectPlayer_ID=2;
    int playerOrOscillatorButtons=1;
    bool playerOrOscillat;
    
    
    bool hearFFTinversedSignal;
    std::vector<float> signalToFFT;
    int fftOutputIndex;
    double fPi;
    bool oneOfTwo=true;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
