/*
  ==============================================================================

    fftInterface.cpp
    Created: 20 Apr 2018 8:58:04pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FFTInterface.h"

//==============================================================================
FFTInterface::FFTInterface(AudioAppComponent *wAudioApp)
{
    wAudioApplication = wAudioApp;
    wSampleRate=0;
    isPitchShiftON = false;
    
    addAndMakeVisible(&fftBufSizeEdit);
    fftBufSizeEdit.setVisible(false);
    fftBufSizeEdit.setEditable(true);
    fftBufSizeEdit.setJustificationType(Justification::centred);
    fftBufSizeEdit.addListener(this);
    
    addAndMakeVisible(&fftBufSizeEditDescript);
    fftBufSizeEditDescript.setVisible(false);
    fftBufSizeEditDescript.attachToComponent(&fftBufSizeEdit, true);
    fftBufSizeEditDescript.setText("Set buffer size: ", dontSendNotification);
    fftBufSizeEditDescript.setJustificationType(Justification::centredRight);
    
    addAndMakeVisible(&selectMatrixFFT);
    selectMatrixFFT.setRadioGroupId(fftSelectorButtons);
    selectMatrixFFT.onClick = [this] { updateToggleState(&selectMatrixFFT, selectMatrixFFT_ID); };
    selectMatrixFFT.setButtonText("Matrix FFT");
    

    addAndMakeVisible(&selectRadix2FFT);
    selectRadix2FFT.setRadioGroupId(fftSelectorButtons);
    selectRadix2FFT.onClick = [this] { updateToggleState(&selectRadix2FFT, selectRadix2FFT_ID); };
    selectRadix2FFT.setButtonText("Radix-2 FFT");
    
    addAndMakeVisible(&wInverseFFT);
    wInverseFFT.onClick = [this] { updateToggleState(&wInverseFFT, wInverse_ID); };
    wInverseFFT.setButtonText("INVERSE");
    wInverseFFT.setVisible(false);
    addAndMakeVisible(&alreadyInversed);
    alreadyInversed.setVisible(false);
    alreadyInversed.setJustificationType(Justification::centredTop);
    alreadyInversed.setEditable(false);
    alreadyInversed.setText("Forw FFT->\nPitct shift->\nBack FFT", dontSendNotification);

    addAndMakeVisible(&turnOFF);
    turnOFF.setButtonText("OFF");
    turnOFF.setRadioGroupId(fftSelectorButtons);
    turnOFF.onClick = [this] { updateToggleState(&turnOFF, turnOFF_ID); };


    addAndMakeVisible(&zeroPadding);
    zeroPadding.setVisible(false);
    zeroPadding.setToggleState(true, dontSendNotification);
    zeroPadding.onClick = [this] { updateToggleState(&zeroPadding, zeroPadding_ID); };
    zeroPadding.setButtonText("Zeros padding");

    addAndMakeVisible(&zerosPaddingDescript);
    zerosPaddingDescript.setVisible(false);
    zerosPaddingDescript.setReadOnly(true);
    zerosPaddingDescript.setMultiLine(true);
    zerosPaddingDescript.setCaretVisible(false);

    zerosPaddingDescript.setText("Uses all 512\nof samples");

    
    addAndMakeVisible(&filterSetLowEnd);
    addAndMakeVisible(&filterSetTopEnd);
    addAndMakeVisible(&filtersDescript);
    filterSetLowEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetLowEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetLowEnd.setTextValueSuffix(" Hz");
    filterSetLowEnd.addListener(this);
    filterSetTopEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetTopEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetTopEnd.setTextValueSuffix(" Hz");
    filterSetTopEnd.addListener(this);
    filterSetLowEnd.setVisible(false);
    filterSetTopEnd.setVisible(false);
    filtersDescript.setVisible(false);
    
    addAndMakeVisible(&linkFilters);
    linkFilters.onClick = [this] { updateToggleState(&linkFilters, linkFilters_ID); };
    linkFilters.setVisible(false);
    addAndMakeVisible(&linkFiltersLabel);
    linkFiltersLabel.setText("link", dontSendNotification);
    linkFiltersLabel.setEditable(false);
    linkFiltersLabel.setVisible(false);
    
    addAndMakeVisible(&setPhase);
    setPhase.setVisible(false);
    setPhaseLabel.setText("Phase", dontSendNotification);
    setPhaseLabel.setJustificationType(Justification::centredBottom);
    setPhaseLabel.attachToComponent(&setPhase, false);
    setPhase.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setPhase.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    setPhase.setTextValueSuffix(" *i^x");
    setPhase.addListener(this);
    setPhase.setRange(0.0, 8.0, 0.01);
    setPhase.setValue(0.0);
    

    addAndMakeVisible(&wWindowBut);
    wWindowBut.setVisible(false);
    wWindowBut.setButtonText("Windowing & overlap");
    wWindowBut.onClick = [this] { updateToggleState(&wWindowBut, winHann_ID); };
    
    addAndMakeVisible(&alreadyWindow);
    alreadyWindow.setVisible(false);
    alreadyWindow.setJustificationType(Justification::centredTop);
    alreadyWindow.setEditable(false);
    alreadyWindow.setText("Signal windowed & overlaped (Hann)", dontSendNotification);

    
    filtersDescript.setEditable(false);
    filtersDescript.setJustificationType(Justification::centred);
    
    addAndMakeVisible(&matrixSizeInfo);
    matrixSizeInfo.setVisible(false);
    matrixSizeInfo.setEditable(false);
    matrixSizeInfo.setJustificationType(Justification::topLeft);
    addAndMakeVisible(&matrixDividerEdit);
    matrixDividerEdit.setVisible(false);
    matrixDividerEdit.setEditable(true);
    matrixDividerEdit.setJustificationType(Justification::centredLeft);
    matrixDividerEdit.addListener(this);
    addAndMakeVisible(&matrixDividerEditDescript);
    matrixDividerEditDescript.setVisible(false);
    matrixDividerEditDescript.setEditable(false);
    matrixDividerEditDescript.setText("Divider (click to change)", dontSendNotification);
    matrixDividerEditDescript.setJustificationType(Justification::centredLeft);
    
    pauseGetNextAudioBlock = false;
}

FFTInterface::~FFTInterface()
{
}


void FFTInterface::setReferences                (CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph, AudioPlayer &player, WavesGen &oscill)
{
    calculator_FFT = &fftCalc;
    oscPan = &osPan;
    graphAnalyser = &graph;
    audioPlayer = &player;
    oscillator = &oscill;
}


void FFTInterface::timerCallback()
{
    stopTimer();
    
    if(whatIsChanged_ID==turnOFF_ID)
    {
        setOFF_fft();
    }
    else if (whatIsChanged_ID==selectMatrixFFT_ID)
    {
        setON_matrixfft();
    }
    else if (whatIsChanged_ID==selectRadix2FFT_ID)
    {
        setON_radix2fft();
    }
    
    else if (whatIsChanged_ID==zeroPadding_ID)
    {
        if(!zeroPadding.getToggleState())
        {
            calculator_FFT->radix2_FFT.setZeroPadding(false);
        }
        else
        {
            calculator_FFT->radix2_FFT.setZeroPadding(true);
        }
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer);
        refresh();
    }
    
    
    
    else if (whatIsChanged_ID==wInverse_ID)
    {
        setInverse_fft();
    }
    
    else if (whatIsChanged_ID==fftBufSizeEdit_ID)
    {
        double temporaryBuf = fftBufSizeEdit.getText().getDoubleValue();
        if(temporaryBuf >= 20.0 && temporaryBuf <= wSampleRate)
            newBufferSize = temporaryBuf;
        else if (temporaryBuf < 20.0)
            newBufferSize = 20.0;
        else if (temporaryBuf > wSampleRate)
            newBufferSize = wSampleRate;
        else
            newBufferSize = 512.0;
        
        rememberedBuffer = newBufferSize;
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize);
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, newBufferSize);
        
        refresh();
    }
    
    
    else if(whatIsChanged_ID==matrixDividerEdit_ID)
    {
        if(wInverseFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDividerEdit.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer);
        }
        else
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDividerEdit.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer);
        }
        matrixSizeInfo.setText(matrixDimToString(), dontSendNotification);
        refresh();
    }
    
    
    else if(whatIsChanged_ID==winHann_ID)
    {
        if(wWindowBut.getToggleState())
        {
            calculator_FFT->isWindowed = true;
            calculator_FFT->mixedRadix_FFT.setWindowing(true);
        }
        else
        {
            calculator_FFT->isWindowed = false;
            calculator_FFT->mixedRadix_FFT.setWindowing(false);
        }
        refresh();
    }
    else
        refresh();
}






void FFTInterface::paint                    (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    
    g.drawRect(fftBufSizeEditBox);
    
    if(selectMatrixFFT.getToggleState())
    {
        g.drawRect(matrixDividerEditBox);
        g.drawRect(matrixSizeInfoBox);
    }
}

void FFTInterface::resized                  ()
{
    fftBufSizeEditBox.setBounds          (103, 7, 80, 25);
    fftBufSizeEdit.setBounds             (103, 7, 80, 25);

    zeroPadding.setBounds                (200, 5, 100, 20);
    zerosPaddingDescript.setBounds       (200, 28, 160, 35);

    filterSetLowEnd.setBounds            (195, 87, 70, 50);
    filterSetTopEnd.setBounds            (295, 87, 70, 50);
    filtersDescript.setBounds            (175, 135, 210, 25);
    linkFilters.setBounds                (268, 110, 20, 16);
    linkFiltersLabel.setBounds           (263, 95, 30, 10);
    
    setPhase.setBounds                   (390, (getHeight()/2)-55, 80, 80);
    wWindowBut.setBounds                    (385, getHeight()-50, 90, 50);
    
    alreadyWindow.setColour              (Label::textColourId, Colours::red);
    alreadyWindow.setBounds              (380, getHeight()-50, 100, 60);


    matrixDividerEditBox.setBounds       (200, 3, 30, 25);
    matrixDividerEdit.setBounds          (201, 3, 28, 28);
    matrixDividerEditDescript.setBounds  (233, 3, 131, 25);
    matrixSizeInfoBox.setBounds          (200, 30, 160, 50);
    matrixSizeInfo.setBounds             (200, 30, 160, 50);
    
    selectMatrixFFT.setBounds            (65, 35, 80, 30);
    selectRadix2FFT.setBounds            (65, 65, 80, 30);
//    selectRegDFT.setBounds               (65, 95, 80, 30);
    turnOFF.setBounds                    (10, 40, 50, 110);
    selectMatrixFFT.changeWidthToFitText();
    selectRadix2FFT.changeWidthToFitText();
//    selectRegDFT.changeWidthToFitText();
    
    wInverseFFT.setBounds                (85, 125, 80, 30);
    
    alreadyInversed.setColour            (Label::textColourId, Colours::red);
    alreadyInversed.setBounds            (65, 110, 125, 60);
}





void FFTInterface::sliderValueChanged       (Slider *slider)
{
    if(slider == &filterSetLowEnd)
    {
        calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
        calculator_FFT->mixedRadix_FFT.setLowEnd(filterSetLowEnd.getValue());
        calculator_FFT->radix2_FFT.setLowEnd(filterSetLowEnd.getValue());
        
        if(areFiltersLinked)
        {
            filterSetTopEnd.setValue(filterSetLowEnd.getValue()+filterDiff);
            calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->mixedRadix_FFT.setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->radix2_FFT.setTopEnd(filterSetTopEnd.getValue());
        }
        else if(filterSetLowEnd.getValue()+3.0 > filterSetTopEnd.getValue()   &&   !areFiltersLinked)
        {
            filterSetTopEnd.setValue(filterSetLowEnd.getValue()+2.0);
            calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->mixedRadix_FFT.setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->radix2_FFT.setTopEnd(filterSetTopEnd.getValue());
        }
        
        int tempIndex;
        if(selectMatrixFFT.getToggleState()) {
            tempIndex = round(filterSetLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
        }
        else if(selectRadix2FFT.getToggleState()) {
            tempIndex = round(filterSetLowEnd.getValue()*(    calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
        }
        else
            return;
        
        if(round(filterSetLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate))>=0)
        {
            for(int k=tempIndex; k>0; k--)
            {
                calculator_FFT->backFFTout[k-1] = 0.0f;
            }
        }
    }
    
    if(slider == &filterSetTopEnd)
    {
        calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
        calculator_FFT->mixedRadix_FFT.setTopEnd(filterSetTopEnd.getValue());
        calculator_FFT->radix2_FFT.setTopEnd(filterSetTopEnd.getValue());
        
        if(areFiltersLinked)
        {
            filterSetLowEnd.setValue(filterSetTopEnd.getValue()-filterDiff);
            calculator_FFT->mixedRadix_FFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->radix2_FFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
        }
        else if(filterSetTopEnd.getValue()-3.0 < filterSetLowEnd.getValue()   &&   !areFiltersLinked)
        {
            filterSetLowEnd.setValue(filterSetTopEnd.getValue()-2.0);
            calculator_FFT->mixedRadix_FFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->radix2_FFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
        }
        
        int tempIndex;
        int highEndInd;
        
        if(selectMatrixFFT.getToggleState()) {
            tempIndex = round(filterSetTopEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
            highEndInd = calculator_FFT->mixedRadix_FFT.getBufferSize()/2;
        }
        else if(selectRadix2FFT.getToggleState()) {
            tempIndex = round(filterSetTopEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
            highEndInd = calculator_FFT->radix2_FFT.getBufferSize()/2;
        }
        else
            return;
        
        if(tempIndex <= wSampleRate/2 )
        {
            for(int k=tempIndex; k<highEndInd; k++)
            {
                calculator_FFT->backFFTout[k] = 0.0f;
            }
        }
    }
    
    
    if(slider == &setPhase)
    {
        calculator_FFT->mixedRadix_FFT.setPhase(setPhase.getValue());
        calculator_FFT->radix2_FFT.setPhase(setPhase.getValue());
    }
}

void FFTInterface::labelTextChanged         (Label *labelThatHasChanged)
{
    pauseFFT(false);
    
    if(labelThatHasChanged == &fftBufSizeEdit)
    {
        whatIsChanged_ID = fftBufSizeEdit_ID;
        startTimer(1000);
    }
    else if(labelThatHasChanged == &matrixDividerEdit)
    {
        whatIsChanged_ID = matrixDividerEdit_ID;
        startTimer(1000);
    }
}





void FFTInterface::updateToggleState        (Button* button, int fftIdentifier)
{
    if(button->getRadioGroupId() == fftSelectorButtons   &&   button->getToggleState())
    {
        switch (fftIdentifier)
        {
            case 1: // MIXED RADIX
                whatIsChanged_ID = selectMatrixFFT_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case 2: // RADIX-2
                whatIsChanged_ID = selectRadix2FFT_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            default:
                return;
        }
    }
    else
    {
        switch (fftIdentifier)
        {
            case 0: // FFT OFF
                std::cout << "dupa" << std::endl;
                whatIsChanged_ID = turnOFF_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case 3: // RADIX-2 - SET ZERO PADDING
                whatIsChanged_ID = zeroPadding_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case 4: // INVERSE
                whatIsChanged_ID = wInverse_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case 5: // WINDOWING & OVERLAPING
                whatIsChanged_ID = winHann_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
    //            setWindowing();
                break;
                
            case 20:
                if(linkFilters.getToggleState())
                {
                    filterDiff = filterSetTopEnd.getValue() - filterSetLowEnd.getValue();
                    areFiltersLinked = true;
                }
                else
                    areFiltersLinked = false;
                break;
                
            default:
                return;
        }
    }
}



void FFTInterface::setSampleRate            (double sample_rate)
{
    wSampleRate = sample_rate;
    filterSetLowEnd.setRange(0.0, wSampleRate/2.0, 1.0);
    filterSetLowEnd.setValue(0.0);
//    filterSetLowEnd.setSkewFactorFromMidPoint(1000);
    
    filterSetTopEnd.setRange(0.0, wSampleRate/2.0, 1.0);
    filterSetTopEnd.setValue(wSampleRate/2.0);
//    filterSetTopEnd.setSkewFactorFromMidPoint(1000);
}

void FFTInterface::setBufferSize(double buffer_size)
{
    newBufferSize = buffer_size;
    rememberedBuffer = newBufferSize;
    fftBufSizeEdit.setText(to_string((int)buffer_size), dontSendNotification);
}





void FFTInterface::setOFF_fft               ()
{
    
    fftBufSizeEdit.setVisible(false);
    fftBufSizeEditDescript.setVisible(false);
    matrixDividerEdit.setVisible(false);
    matrixSizeInfo.setVisible(false);
    matrixDividerEditDescript.setVisible(false);
    selectMatrixFFT.setToggleState(false, NotificationType::dontSendNotification);
    alreadyInversed.setVisible(false);
    alreadyWindow.setVisible(false);
    wWindowBut.setVisible(false);
    
    selectRadix2FFT.setToggleState(false, NotificationType::dontSendNotification);
    zeroPadding.setVisible(false);
    zerosPaddingDescript.setVisible(false);
    
    filterSetTopEnd.setVisible(false);
    filterSetLowEnd.setVisible(false);
    setPhase.setVisible(false);
    wInverseFFT.setVisible(false);
    filtersDescript.setVisible(false);
    linkFiltersLabel.setVisible(false);
    linkFilters.setVisible(false);

    calculator_FFT->isForward = true;
    calculator_FFT->isWindowed = false;
    pauseGetNextAudioBlock = false;
    oscillator->selectWave(rememberedWaveType);
    graphAnalyser->isFFTon=false;
    if(!graphAnalyser->isTimerRunning())
        graphAnalyser->startTimer(40);
    repaint();
}

void FFTInterface::setON_matrixfft          ()
{
    if(selectMatrixFFT.getToggleState())
    {
        setVisibleFiltersAndBuffSize();
        rememberedInvPitchWin();

        double tempBuf = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);

        graphAnalyser->setLowEndIndex();
        repaint();
        graphAnalyser->isFFTon=true;
        if(!graphAnalyser->isTimerRunning())
            graphAnalyser->startTimer(40);

        pauseGetNextAudioBlock = false;
        calculator_FFT->dataIsReadyToFFT = true;
        oscillator->selectWave(rememberedWaveType);
        calculator_FFT->selectFFT(1);
    }
}



void FFTInterface::setON_radix2fft          ()
{
    if(selectRadix2FFT.getToggleState())
    {
        setVisibleFiltersAndBuffSize();
        rememberedInvPitchWin();

        double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        zerosPaddingDescript.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                                  rememberedBuffer, tempBuf-rememberedBuffer));
        
//        std::cout << tempBuf << std::endl;
        
        graphAnalyser->setLowEndIndex();
        repaint();
        graphAnalyser->isFFTon=true;
        if(!graphAnalyser->isTimerRunning())
            graphAnalyser->startTimer(40);
        
        pauseGetNextAudioBlock = false;
        calculator_FFT->dataIsReadyToFFT = true;
        oscillator->selectWave(rememberedWaveType);
        calculator_FFT->selectFFT(2);
    }
}



void FFTInterface::setInverse_fft           ()
{
    if(wInverseFFT.getToggleState())
    {
        wWindowBut.setVisible(true);
        setPhase.setVisible(true);
        calculator_FFT->isForward = false;
//        graphAnalyser->isForward = false;
        alreadyWindow.setVisible(false);
        alreadyWindow.setVisible(false);
    }
    else
    {
        wWindowBut.setVisible(false);
        setPhase.setVisible(false);
        calculator_FFT->isForward = true;
//        graphAnalyser->isForward = true;
        alreadyWindow.setVisible(false);
        alreadyWindow.setVisible(false);
    }

    refresh();
}


void FFTInterface::pauseFFT(bool pauseFALSE_ResumeTRUE)
{
    rememberedWaveType=oscillator->getWaveType();

    if(graphAnalyser->isTimerRunning())
        graphAnalyser->stopTimer();
    
    pauseGetNextAudioBlock = true;
    calculator_FFT->dataIsReadyToFFT = false;
    calculator_FFT->selectFFT(0);
    oscillator->selectWave(0);
    rememberInvWasClicked = wInverseFFT.getToggleState();
    remembereWinWasClicked=wWindowBut.getToggleState();
}

void FFTInterface::refresh                  ()
{
    if(selectMatrixFFT.getToggleState())
        setON_matrixfft();
    else if(selectRadix2FFT.getToggleState())
        setON_radix2fft();
//    else if(selectRegDFT.getToggleState())
//        setON_regular_DFT();
    else
        return;
}


void FFTInterface::rememberedInvPitchWin()
{
    if(!isPitchShiftON)
    {
        wInverseFFT.setVisible(true);
        wWindowBut.setVisible(true);
    }
    else
    {
        alreadyWindow.setVisible(true);
        alreadyInversed.setVisible(true);
        wWindowBut.setVisible(false);
    }
    
    if(rememberInvWasClicked) {
        calculator_FFT->isForward = false;
        setPhase.setVisible(true);
    }
    else {
        calculator_FFT->isForward = true;
        setPhase.setVisible(false);
    }
    
    if(remembereWinWasClicked || isPitchShiftON) {
        calculator_FFT->isWindowed = true;
        
        filterSetLowEnd.setVisible(true);
        filterSetTopEnd.setVisible(true);
        filtersDescript.setVisible(true);
        linkFilters.setVisible(true);
        linkFiltersLabel.setVisible(true);
        filtersDescript.setText("Change sound only when INVERSED", dontSendNotification);
    }
    else {
        calculator_FFT->isWindowed = false;
        
        filterSetLowEnd.setVisible(false);
        filterSetTopEnd.setVisible(false);
        filtersDescript.setVisible(false);
        linkFilters.setVisible(false);
        linkFiltersLabel.setVisible(false);
    }
}

void FFTInterface::setVisibleFiltersAndBuffSize()
{
    fftBufSizeEdit.setVisible(true);
    fftBufSizeEditDescript.setVisible(true);
    
    if(selectMatrixFFT.getToggleState())
    {
        zerosPaddingDescript.setVisible(false);
        zeroPadding.setVisible(false);
        matrixSizeInfo.setVisible(true);
        matrixDividerEdit.setVisible(true);
        matrixDividerEditDescript.setVisible(true);
        matrixSizeInfo.setText(matrixDimToString(), dontSendNotification);
        matrixDividerEdit.setText(to_string(calculator_FFT->mixedRadix_FFT.getRadDivider()), dontSendNotification);
    }
    else if(selectRadix2FFT.getToggleState())
    {
        zerosPaddingDescript.setVisible(true);
        zeroPadding.setVisible(true);
        matrixSizeInfo.setVisible(false);
        matrixDividerEdit.setVisible(false);
        matrixDividerEditDescript.setVisible(false);
    }
}

string FFTInterface::matrixDimToString      ()
{
    vector<float> matDim = calculator_FFT->mixedRadix_FFT.getRadDimensions();
    string wEqual = ";  ";
    string dimensions = "Radix dim.:\n";

    for(int i=0; i<matDim.size(); i++)
    {
        dimensions += (to_string((int)matDim[i]) + wEqual);
    }
    
    return dimensions;
}



string FFTInterface::setZerosInfo           (int use, int bufSize, int zero)
{
    string uses = "Uses ";
    int used = use;
    string offf = " of ";
    int buf = bufSize;
    string samplll = " samples";
    
    if(zero>0)
    {
        string andd = " plus ";
        int zer = zero;
        string seros = " zeros";
        
        return uses + to_string(used) + offf + to_string(buf) + samplll + andd +to_string(zer) + seros;
    }
    
    return uses + to_string(used) + offf + to_string(buf) + samplll;
}

