/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscInterface.h"
#include "AudioPlayer.h"
#include "FFTInterface.h"
#include "PitchShiftingGUI.h"
#include "WavesGen.h"
#include "Display_Logarithmic.h"
#include "Display_Linear.h"
#include "CalculateDTFT.h"
#include "GraphAnalyser.h"
#include "Clock.h"
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, private Timer /*, private Slider::Listener */
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    
    void timerCallback() override;
    
    enum ButtonID {
        graphOff_ID,
        selectOscill_ID,
        selectPlayer_ID,
        freqDisp_ID,
        timeDisp_ID,
        waveDisp_ID,
        d_weightingDisp_ID
    };
    
    void updateToggleState(Button* button, ButtonID buttonID);

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    
    //==============================================================================
    void playWaveGen(const AudioSourceChannelInfo& bufferToFill);
    void playInversedFFTWaveGen(const AudioSourceChannelInfo& bufferToFill);
    
    void playIAudioFile(const AudioSourceChannelInfo& bufferToFill);
    void playInversedFFTAudioFile(const AudioSourceChannelInfo& bufferToFill);
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void fft_defaultSettings();
    void calculateTime();

    Display_Logarithmic display_logarithmic;
    Display_Linear      display_linear;
private:
    AudioDeviceSelectorComponent adsc;
    //==============================================================================
    // Your private member variables go here...
    CalculateDTFT calculator_FFT;
    AudioBuffer<float> tempBuff;
    

    GraphAnalyser graphAnalyser;
    FFTInterface fftInterface;
    OscInterface oscInterface;
    WavesGen oscillator;
    AudioPlayer wAudioPlayer;
    PitchShiftingGUI pitchShiftGui;
    
    double wSampleRate;
    double deviceBufferSize;
    
    ToggleButton selectOscill;
    ToggleButton selectPlayer;
    int playerOrOscillatorButtons=1;
    bool playerOrOscillat;
    
    int fftTimeElapsed=0;
    int bufferCounter=0;
    
    Label fftTimeElapsedLabel;
    Label fftTimeElapsedInfo;
    
    bool hearFFTinversedSignal;
    std::vector<float> signalToFFT;
    int fftOutputIndex;
    double fPi;
    
    ToggleButton graphOff;
    ToggleButton freqDisp;
    ToggleButton timeDisp;
    ToggleButton waveDisp;
    ToggleButton d_weightingDisp;
    
    Atomic<double> &audioPosition;
    Atomic<double> &audioVolume;
    
    int selectorFreqTimeButton = 100;
    Atomic<bool> &isPositionChanged, &isVolumeChanged;
    
    long tempp=0;
    int ttt=0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
