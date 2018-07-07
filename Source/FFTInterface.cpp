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
    selectMatrixFFT.setRadioGroupId(fftRadioButtonsID);
    selectMatrixFFT.onClick = [this] { updateToggleState(&selectMatrixFFT, selectMatrixFFT_ID); };
    selectMatrixFFT.setButtonText("Matrix FFT");
    

    addAndMakeVisible(&selectRadix2FFT);
    selectRadix2FFT.setRadioGroupId(fftRadioButtonsID);
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
//    alreadyInversed.setText("Forw FFT->\nPitct shift->\nBack FFT", dontSendNotification);
    alreadyInversed.setText("signal\nINVERSED", dontSendNotification);

    addAndMakeVisible(&turnOFF);
    turnOFF.setButtonText("OFF");
    turnOFF.setRadioGroupId(fftRadioButtonsID);
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
    
    addAndMakeVisible(&rad2FIFO);
    rad2FIFO.setVisible(false);
    rad2FIFO.setToggleState(false, dontSendNotification);
    rad2FIFO.onClick = [this] { updateToggleState(&rad2FIFO, rad2FIFO_ID); };
    rad2FIFO.setButtonText("fifo");

    
    addAndMakeVisible(&filterSetLowEnd);
    addAndMakeVisible(&filterSetTopEnd);
    addAndMakeVisible(&filtersDescript);
    filtersDescript.setText("Change sound when INVERSED", dontSendNotification);
    filterSetLowEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetLowEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetLowEnd.textFromValueFunction = [](double value) {
        double freq = pow(10.0, value*log10(22050.0)/22050.0);
        return juce::String(floor(freq)) + "Hz";
    };
    filterSetLowEnd.valueFromTextFunction = [](String value) {
        double freq = log10(value.getDoubleValue()) * 22050.0/log10(22050.0);
        return freq;
    };
    filterSetLowEnd.addListener(this);
    filterSetTopEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetTopEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetTopEnd.textFromValueFunction = [](double value) {
        double freq = pow(10.0, value*log10(22050.0)/22050.0);
        return juce::String(ceil(freq)) + "Hz";
    };
    filterSetTopEnd.valueFromTextFunction = [](String value) {
        double freq = log10(value.getDoubleValue()) * 22050.0/log10(22050.0);
        return freq;
    };
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
    setPhase.setTextBoxStyle(Slider::TextBoxBelow, false, 75, 20);
    setPhase.textFromValueFunction = [](float value) {
            return "j^" + juce::String(value, 2);
    };
    setPhase.addListener(this);
    setPhase.setRange(0.0, 4.0, 0.01);
    setPhase.setValue(0.0);
    

    addAndMakeVisible(&wWindowBut);
    wWindowBut.setVisible(false);
    wWindowBut.setButtonText("Windowing");
    wWindowBut.onClick = [this] { updateToggleState(&wWindowBut, winHann_ID); };
    
    addAndMakeVisible(&setWindowOverLap);
    setWindowOverLap.setVisible(false);
    setWindowOverLap.setRange(1, 16, 1);
    setWindowOverLap.setValue(4);
    setWindowOverLap.setTextBoxStyle(Slider::TextBoxLeft, false, 25, 15);
    setWindowOverLap.addListener(this);
    
    addAndMakeVisible(&alreadyWindow);
    alreadyWindow.setVisible(false);
    alreadyWindow.setJustificationType(Justification::centredTop);
    alreadyWindow.setEditable(false);
    alreadyWindow.setText("Signal\nWINDOWED", dontSendNotification);
    
    addAndMakeVisible(&setWindowOverLapLabel);
    setWindowOverLapLabel.setVisible(false);
    setWindowOverLapLabel.setText("overlapping:", dontSendNotification);
    setWindowOverLapLabel.setJustificationType(Justification::centred);
    
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


void FFTInterface::setReferences                (CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph, AudioPlayer &player, WavesGen &oscill, Display_Linear &displLinea)
{
    calculator_FFT = &fftCalc;
    oscPan = &osPan;
    graphAnalyser = &graph;
    audioPlayer = &player;
    oscillator = &oscill;
    dispLine = &displLinea;
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
    
    else if (whatIsChanged_ID==rad2FIFO_ID)
    {
        if(!rad2FIFO.getToggleState())
        {
            oscillator->selectWave(rememberedWaveType);
            zeroPadding.setVisible(true);
            updateToggleState(&zeroPadding, zeroPadding_ID);
        }
        else
        {
            zeroPadding.setVisible(false);
//            calculator_FFT->radix2_FFT.setZeroPadding(true);
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, twoPowerToInt(rememberedBuffer));
            refresh();
        }
    }
    
    
    else if (whatIsChanged_ID==wInverse_ID)
    {
        setInverse_fft();
    }
    
    else if (whatIsChanged_ID==fftBufSizeEdit_ID)
    {
        double temporaryBuf = fftBufSizeEdit.getText().getDoubleValue();
        if(temporaryBuf >= 16.0 && temporaryBuf <= wSampleRate)
            newBufferSize = temporaryBuf;
        else if (temporaryBuf < 16.0)
            newBufferSize = 16.0;
        else if (temporaryBuf > wSampleRate)
            newBufferSize = wSampleRate;

        
        rememberedBuffer = newBufferSize;
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize);
        
        if(rad2FIFO.getToggleState())
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, twoPowerToInt(rememberedBuffer));
        else
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
            calculator_FFT->radix2_FFT.setWindowing(true);
            setWindowOverLap.setVisible(true);
            setWindowOverLapLabel.setVisible(true);
        }
        else
        {
            calculator_FFT->isWindowed = false;
            calculator_FFT->mixedRadix_FFT.setWindowing(false);
            calculator_FFT->radix2_FFT.setWindowing(false);
            setWindowOverLap.setVisible(false);
            setWindowOverLapLabel.setVisible(false);
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
//    g.drawLine(150, 10, 150, getHeight()-10);
    Line<float> separator(385, 10, 385,getHeight()-10);
    float dashLength[2] = { 4, 10 };
    g.drawDashedLine(separator, dashLength, 2);

    g.setColour (Colours::white);
    
    if(calculator_FFT->fftType != 0)
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
    rad2FIFO.setBounds                   (310, 5, 100, 20);

    filterSetLowEnd.setBounds            (195, 87, 70, 50);
    filterSetTopEnd.setBounds            (295, 87, 70, 50);
    filtersDescript.setBounds            (175, 135, 210, 25);
    linkFilters.setBounds                (268, 110, 20, 16);
    linkFiltersLabel.setBounds           (263, 95, 30, 10);
    
    setPhase.setBounds                   (407, (getHeight()/2)-55, 67, 67);
    wWindowBut.setBounds                 (390, getHeight()-72, 90, 50);
    setWindowOverLap.setBounds           (390, getHeight()-22, 90, 20);
    
    alreadyWindow.setColour              (Label::textColourId, Colours::red);
    alreadyWindow.setBounds              (385, getHeight()-67, 100, 40);
    setWindowOverLapLabel.setBounds      (385, getHeight()-37, 90, 15);


    matrixDividerEditBox.setBounds       (200, 3, 30, 25);
    matrixDividerEdit.setBounds          (201, 3, 28, 28);
    matrixDividerEditDescript.setBounds  (233, 3, 131, 25);
    matrixSizeInfoBox.setBounds          (200, 30, 160, 50);
    matrixSizeInfo.setBounds             (200, 30, 160, 50);
    
    selectMatrixFFT.setBounds            (65, 35, 80, 30);
    selectRadix2FFT.setBounds            (65, 65, 80, 30);
    turnOFF.setBounds                    (10, 40, 50, 110);
    selectMatrixFFT.changeWidthToFitText();
    selectRadix2FFT.changeWidthToFitText();
    
    wInverseFFT.setBounds                (85, 125, 80, 30);
    
    alreadyInversed.setColour            (Label::textColourId, Colours::red);
    alreadyInversed.setBounds            (65, 120, 125, 60);
}





void FFTInterface::sliderValueChanged       (Slider *slider)
{
    if((slider == &filterSetLowEnd) || slider == &filterSetTopEnd) {
        
        if(slider == &filterSetLowEnd)
        {
            if(areFiltersLinked)
            {
                filterSetTopEnd.setValue(filterSetLowEnd.getValue()+filterDiff);
            }
            else if(filterSetLowEnd.getValue()+3.0 > filterSetTopEnd.getValue()   &&   !areFiltersLinked)
            {
                filterSetTopEnd.setValue(filterSetLowEnd.getValue()+2.0);
            }
        }
        
        if(slider == &filterSetTopEnd)
        {
            if(areFiltersLinked)
            {
                filterSetLowEnd.setValue(filterSetTopEnd.getValue()-filterDiff);
            }
            else if(filterSetTopEnd.getValue()-3.0 < filterSetLowEnd.getValue()   &&   !areFiltersLinked)
            {
                filterSetLowEnd.setValue(filterSetTopEnd.getValue()-2.0);
            }
        }
        
        float loEnd = pow(10.0, filterSetLowEnd.getValue()*log10(22050.0)/22050.0);
        float toEnd = pow(10.0, filterSetTopEnd.getValue()*log10(22050.0)/22050.0);
        calculator_FFT->mixedRadix_FFT.setLowEnd(loEnd);
        calculator_FFT->radix2_FFT.setLowEnd(loEnd);
        calculator_FFT->mixedRadix_FFT.setTopEnd(toEnd);
        calculator_FFT->radix2_FFT.setTopEnd(toEnd);
        
        int filterLowEndIndex;
        int filterTopEndIndex;
        int topEndInd;
        
        if(selectMatrixFFT.getToggleState()) {
            filterLowEndIndex = round(loEnd*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
            filterTopEndIndex = round(toEnd*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
            topEndInd = calculator_FFT->mixedRadix_FFT.getBufferSize()/2;
        }
        else if(selectRadix2FFT.getToggleState()) {
            filterLowEndIndex = round(loEnd*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
            filterTopEndIndex = round(toEnd*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
            topEndInd = calculator_FFT->radix2_FFT.getBufferSize()/2;
        }
        else
            return;
        
        if(filterLowEndIndex>=0)
        {
            for(int k=filterLowEndIndex; k>0; k--)
            {
                calculator_FFT->backFFTout[k-1] = 0.0f;
            }
        }
        
        if(filterTopEndIndex <= wSampleRate/2 )
        {
            for(int k=filterTopEndIndex; k<topEndInd; k++)
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
    
    if(slider == &setWindowOverLap)
    {
        calculator_FFT->overLap = setWindowOverLap.getValue();
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





void FFTInterface::updateToggleState        (Button* button, ButtonID buttonID)
{
    if(button->getRadioGroupId() == fftRadioButtonsID   &&   button->getToggleState())
    {
        switch (buttonID)
        {
            case selectMatrixFFT_ID: // MIXED RADIX
                whatIsChanged_ID = selectMatrixFFT_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case selectRadix2FFT_ID: // RADIX-2
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
        switch (buttonID)
        {
            case turnOFF_ID: // FFT OFF
                whatIsChanged_ID = turnOFF_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case zeroPadding_ID: // RADIX-2 - SET ZERO PADDING
                whatIsChanged_ID = zeroPadding_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case rad2FIFO_ID: // RADIX-2 - SET FIFO
                whatIsChanged_ID = rad2FIFO_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case wInverse_ID: // INVERSE
                whatIsChanged_ID = wInverse_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case winHann_ID: // WINDOWING & OVERLAPING
                whatIsChanged_ID = winHann_ID;
                pauseFFT(false);
                startTimer(ceil((calculator_FFT->timeElapsed/1000.0f)*10.0f));
                break;
                
            case linkFilters_ID:
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

    filterDiff = filterSetTopEnd.getValue() - filterSetLowEnd.getValue();
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
    setWindowOverLap.setVisible(false);
    setWindowOverLapLabel.setVisible(false);
    
    selectRadix2FFT.setToggleState(false, NotificationType::dontSendNotification);
    zeroPadding.setVisible(false);
    zerosPaddingDescript.setVisible(false);
    rad2FIFO.setVisible(false);
    
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
//    if(!graphAnalyser->isTimerRunning())
//        graphAnalyser->startTimer(40);
    repaint();
}

void FFTInterface::setON_matrixfft          ()
{
    if(selectMatrixFFT.getToggleState())
    {
        setVisibleFiltersAndBuffSize();
        rememberedInvPitchWin();
        calculator_FFT->selectFFT(1);
        
        double tempBuf = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf, 1);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        dispLine->setBuffSize(tempBuf);
        
        sliderValueChanged(&filterSetTopEnd);
        
        if(dispLine->timeOrWave == 1)
        {
            dispLine->wSampleRateToDisplay = tempBuf;
            dispLine->setZoomRangeTime();
        }
        if(dispLine->timeOrWave == 2)
        {
            dispLine->wSampleRateToDisplay = tempBuf;
            dispLine->setZoomRangeOscil();
        }

        graphAnalyser->setLowEndIndex();
        repaint();
        graphAnalyser->isFFTon=true;
        if(!graphAnalyser->isTimerRunning()   &&   isGraphON)
            graphAnalyser->startTimer(40);

        pauseGetNextAudioBlock = false;
        calculator_FFT->dataIsReadyToFFT = true;
        oscillator->selectWave(rememberedWaveType);
//        calculator_FFT->selectFFT(1);
        calculator_FFT->fftIsReady.set(true);
        oscPan->isGraphOn = true;
    }
}



void FFTInterface::setON_radix2fft          ()
{
    if(selectRadix2FFT.getToggleState())
    {
        setVisibleFiltersAndBuffSize();
        rememberedInvPitchWin();
        calculator_FFT->selectFFT(2);
        
        double tempBufDisc = calculator_FFT->radix2_FFT.getBufferSize();;
        double tempBuf = zeroPadding.getToggleState() ? calculator_FFT->radix2_FFT.getTrueBufferSize() : calculator_FFT->radix2_FFT.getBufferSize();;
        calculator_FFT->setNewBufSize(tempBufDisc, 2);
        graphAnalyser->setNewBufSize(tempBufDisc);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        zerosPaddingDescript.setText(setZerosInfo(!rad2FIFO.getToggleState()?
                                                  (rememberedBuffer>tempBufDisc?tempBufDisc:rememberedBuffer):tempBufDisc,
                                                  !rad2FIFO.getToggleState()?rememberedBuffer:tempBufDisc,
                                                  !rad2FIFO.getToggleState()?(tempBufDisc-rememberedBuffer):0  ));
        

        if(tempBufDisc >= rememberedBuffer)
            dispLine->setBuffSize(rememberedBuffer);
        else
            dispLine->setBuffSize(tempBufDisc);

        sliderValueChanged(&filterSetTopEnd);
        
        if(dispLine->timeOrWave == 1)
        {
            dispLine->wSampleRateToDisplay = tempBufDisc;
            dispLine->setZoomRangeTime();
        }
        if(dispLine->timeOrWave == 2)
        {
            dispLine->wSampleRateToDisplay = tempBufDisc;
            dispLine->setZoomRangeOscil();
        }
        
        graphAnalyser->setLowEndIndex();
        repaint();
        graphAnalyser->isFFTon=true;
        if(!graphAnalyser->isTimerRunning()   &&   isGraphON)
            graphAnalyser->startTimer(40);
        
        pauseGetNextAudioBlock = false;
        calculator_FFT->dataIsReadyToFFT = true;
        oscillator->selectWave(rememberedWaveType);
//        calculator_FFT->selectFFT(2);
        calculator_FFT->fftIsReady.set(true);
        oscPan->isGraphOn = true;
    }
}



void FFTInterface::setInverse_fft           ()
{
    if(wInverseFFT.getToggleState())
    {
        wWindowBut.setVisible(true);
        setPhase.setVisible(true);
        calculator_FFT->isForward = false;
        alreadyWindow.setVisible(false);
        alreadyWindow.setVisible(false);
    }
    else
    {
        wWindowBut.setVisible(false);
        setPhase.setVisible(false);
        calculator_FFT->isForward = true;
        alreadyWindow.setVisible(false);
        alreadyWindow.setVisible(false);
    }

    refresh();
}


void FFTInterface::pauseFFT(bool pauseFALSE_ResumeTRUE)
{
    rememberedWaveType=oscillator->getWaveType();

    if(graphAnalyser->isTimerRunning()  &&  !isGraphON)
        graphAnalyser->stopTimer();
    
    oscPan->isGraphOn = false;
    pauseGetNextAudioBlock = true;
    calculator_FFT->dataIsReadyToFFT = false;
//    calculator_FFT->selectFFT(0);
    calculator_FFT->fftIsReady.set(false);
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
        if(wWindowBut.getToggleState())
        {
            setWindowOverLap.setVisible(true);
            setWindowOverLapLabel.setVisible(true);
        }
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
        if(isPitchShiftON || rememberInvWasClicked)
            filtersDescript.setVisible(false);
        else
            filtersDescript.setVisible(true);
        linkFilters.setVisible(true);
        linkFiltersLabel.setVisible(true);
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
        rad2FIFO.setVisible(false);
        matrixSizeInfo.setVisible(true);
        matrixDividerEdit.setVisible(true);
        matrixDividerEditDescript.setVisible(true);
        matrixSizeInfo.setText(matrixDimToString(), dontSendNotification);
        matrixDividerEdit.setText(to_string(calculator_FFT->mixedRadix_FFT.getRadDivider()), dontSendNotification);
    }
    else if(selectRadix2FFT.getToggleState())
    {
        zerosPaddingDescript.setVisible(true);
        if(!rad2FIFO.getToggleState())
            zeroPadding.setVisible(true);
        rad2FIFO.setVisible(true);
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



double FFTInterface::twoPowerToInt(double &value)
{
    double temp = pow(2, floor(log2(value)));
    if(value > temp)
        return temp*2;
    else
        return value;
}
