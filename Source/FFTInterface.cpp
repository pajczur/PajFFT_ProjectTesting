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
    
    addAndMakeVisible(&fftBufSizeEdit);
    fftBufSizeEdit.setEditable(true);
    fftBufSizeEdit.setText("512", dontSendNotification);
    fftBufSizeEdit.setJustificationType(Justification::centred);
    fftBufSizeEdit.addListener(this);
    
    addAndMakeVisible(&fftBufSizeEditDescript);
    fftBufSizeEditDescript.attachToComponent(&fftBufSizeEdit, true);
    fftBufSizeEditDescript.setText("Set buffer size: ", dontSendNotification);
    fftBufSizeEditDescript.setJustificationType(Justification::centredRight);
    
    addAndMakeVisible(&selectMatrixFFT);
    selectMatrixFFT.setRadioGroupId(fftSelectorButtons);
    selectMatrixFFT.onClick = [this] { updateToggleState(&selectMatrixFFT, matrixFFT_ID); };
    selectMatrixFFT.setButtonText("Matrix FFT");

    addAndMakeVisible(&selectRadix2FFT);
    selectRadix2FFT.setRadioGroupId(fftSelectorButtons);
    selectRadix2FFT.onClick = [this] { updateToggleState(&selectRadix2FFT, radix2FFT_ID); };
    selectRadix2FFT.setButtonText("Radix-2 FFT");
    
    addAndMakeVisible(&selectRegDFT);
    selectRegDFT.setRadioGroupId(fftSelectorButtons);
    selectRegDFT.onClick = [this] { updateToggleState(&selectRegDFT, regDFT_ID); };
    selectRegDFT.setButtonText("Regular DFT");
    
    wInverseFFT.onClick = [this] { updateToggleState(&wInverseFFT, wInverse_ID); };
    wInverseFFT.setButtonText("INVERSE");

    addAndMakeVisible(&turnOFF);
    turnOFF.setButtonText("OFF");
    turnOFF.setRadioGroupId(fftSelectorButtons);
    turnOFF.onClick = [this] { updateToggleState(&turnOFF, turnOFF_ID); };
    
    zeroPadding.setToggleState(true, dontSendNotification);
    zeroPadding.onClick = [this] { updateToggleZeroPad(&zeroPadding, radix2FFT_ID); };
    zeroPadding.setButtonText("Zeros padding");

    zerosPaddingDescript.setReadOnly(true);
    zerosPaddingDescript.setMultiLine(true);
    zerosPaddingDescript.setCaretVisible(false);

    zerosPaddingDescript.setText("Uses all 512\nof samples");
    
    filterSetLowEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetLowEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetLowEnd.setTextValueSuffix(" Hz");
    filterSetLowEnd.addListener(this);
    filterSetTopEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    filterSetTopEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    filterSetTopEnd.setTextValueSuffix(" Hz");
    filterSetTopEnd.addListener(this);
    
    setPhaseLabel.setText("Phase", dontSendNotification);
    setPhaseLabel.setJustificationType(Justification::centredBottom);
    setPhaseLabel.attachToComponent(&setPhase, false);
    setPhase.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setPhase.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    setPhase.setTextValueSuffix(" *i^x");
    setPhase.addListener(this);
    setPhase.setRange(0.0, 8.0, 0.01);
    setPhase.setValue(0.0);
    
    winHann.setButtonText("Hann Window");
    winHann.onClick = [this] { updateToggleState(&winHann, winHann_ID); };
    
    filtersDescript.setEditable(false);
    filtersDescript.setJustificationType(Justification::centred);
    
    matrixSizeInfo.setEditable(false);
    matrixSizeInfo.setJustificationType(Justification::topLeft);
    matrixDividerEdit.setEditable(true);
    matrixDividerEdit.setJustificationType(Justification::centredLeft);
    matrixDividerEdit.addListener(this);
    matrixDividerEditDescript.setEditable(false);
    matrixDividerEditDescript.setText("Divider (click to change)", dontSendNotification);
    matrixDividerEditDescript.setJustificationType(Justification::centredLeft);
    
}

FFTInterface::~FFTInterface()
{
}





void FFTInterface::wSettings                (CalculateDTFT &fftCalc, OscInterface &osPan, GraphAnalyser &graph)
{
    calculator_FFT = &fftCalc;
    oscPan = &osPan;
    graphAnalyser = &graph;
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
    fftBufSizeEditBox.setBounds          (100, 3, 80, 25);
    fftBufSizeEdit.setBounds             (100, 3, 80, 25);

    zeroPadding.setBounds                (200, 5, 100, 20);
    zerosPaddingDescript.setBounds                  (200, 28, 160, 35);

    filterSetLowEnd.setBounds            (200, 87, 70, 50);
    filterSetTopEnd.setBounds            (290, 87, 70, 50);
    filtersDescript.setBounds            (185, 135, 210, 25);
    
    setPhase.setBounds                   (390, (getHeight()/2)-50, 90, 90);
    winHann.setBounds                    (410, getHeight()-33, 55, 25);


    matrixDividerEditBox.setBounds       (200, 3, 30, 25);
    matrixDividerEdit.setBounds          (201, 3, 28, 28);
    matrixDividerEditDescript.setBounds  (233, 3, 131, 25);
    matrixSizeInfoBox.setBounds          (200, 30, 160, 50);
    matrixSizeInfo.setBounds             (200, 30, 160, 50);
    
    selectMatrixFFT.setBounds            (65, 35, 80, 30);
    selectRadix2FFT.setBounds            (65, 65, 80, 30);
    selectRegDFT.setBounds               (65, 95, 80, 30);
    turnOFF.setBounds                    (10, 40, 50, 110);
    selectMatrixFFT.changeWidthToFitText();
    selectRadix2FFT.changeWidthToFitText();
    selectRegDFT.changeWidthToFitText();
    
    wInverseFFT.setBounds                (85, 125, 80, 30);
}





void FFTInterface::sliderValueChanged       (Slider *slider)
{
    if(slider == &filterSetLowEnd)
    {
        if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            
            if(filterSetLowEnd.getValue()+11.0 > filterSetTopEnd.getValue())
            {
                filterSetTopEnd.setValue(filterSetLowEnd.getValue()+10.0);
                calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            }
            if(round(filterSetLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(filterSetLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput->at(k-1) = 0.0f;
                }
            }
        }
        else if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_FFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            
            if(filterSetLowEnd.getValue()+11.0 > filterSetTopEnd.getValue())
            {
                filterSetTopEnd.setValue(filterSetLowEnd.getValue()+10.0);
                calculator_FFT->radix2_FFT.setTopEnd(filterSetTopEnd.getValue());
                calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            }
            if(round(filterSetLowEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(filterSetLowEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput->at(k-1) = 0.0f;
                }
            }
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_DFT.setLowEnd(filterSetLowEnd.getValue());
            calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            
            if(filterSetLowEnd.getValue()+11.0 > filterSetTopEnd.getValue())
            {
                filterSetTopEnd.setValue(filterSetLowEnd.getValue()+10.0);
                calculator_FFT->regular_DFT.setTopEnd(filterSetTopEnd.getValue());
                calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            }
            if(round(filterSetLowEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(filterSetLowEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput->at(k-1) = 0.0f;
                }
            }
        }
        else
        {
            return;
        }
    }
    
    if(slider == &filterSetTopEnd)
    {
        if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            
            if(filterSetTopEnd.getValue()-11.0 < filterSetLowEnd.getValue())
            {
                filterSetLowEnd.setValue(filterSetTopEnd.getValue()-10.0);
                calculator_FFT->mixedRadix_FFT.setLowEnd(filterSetLowEnd.getValue());
                calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            }
            if(round(filterSetTopEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(filterSetTopEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->mixedRadix_FFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput->at(k) = 0.0f;
                }
            }
        }
        else if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_FFT.setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            
            if(filterSetTopEnd.getValue()-11.0 < filterSetLowEnd.getValue())
            {
                filterSetLowEnd.setValue(filterSetTopEnd.getValue()-10.0);
                calculator_FFT->radix2_FFT.setLowEnd(filterSetLowEnd.getValue());
                calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            }
            if(round(filterSetTopEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(filterSetTopEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->radix2_FFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput->at(k) = 0.0f;
                }
            }
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_DFT.setTopEnd(filterSetTopEnd.getValue());
            calculator_FFT->setTopEnd(filterSetTopEnd.getValue());
            
            if(filterSetTopEnd.getValue()-11.0 < filterSetLowEnd.getValue())
            {
                filterSetLowEnd.setValue(filterSetTopEnd.getValue()-10.0);
                calculator_FFT->regular_DFT.setLowEnd(filterSetLowEnd.getValue());
                calculator_FFT->setLowEnd(filterSetLowEnd.getValue());
            }
            if(round(filterSetTopEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(filterSetTopEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->regular_DFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput->at(k) = 0.0f;
                }
            }
        }
        else
        {
            return;
        }
    }
    
    
    if(slider == &setPhase)
    {
        if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_IFFT.setPhase(setPhase.getValue());
        }
        else if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_IFFT.setPhase(setPhase.getValue());
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_IDFT.setPhase(setPhase.getValue());
        }
    }
}

void FFTInterface::labelTextChanged         (Label *labelThatHasChanged)
{
    calculator_FFT->dataIsInUse = true;
    calculator_FFT->dataIsReadyToFFT = false;
    
    
    
    if(labelThatHasChanged == &fftBufSizeEdit)
    {
        double temporaryBuf = fftBufSizeEdit.getText().getDoubleValue();
        if(temporaryBuf >= 20.0 && temporaryBuf <= wSampleRate)
            newBufferSize = fftBufSizeEdit.getText().getDoubleValue();
        else if (temporaryBuf < 20.0)
            newBufferSize = 20.0;
        else if (temporaryBuf > wSampleRate)
            newBufferSize = wSampleRate;
        else
            newBufferSize = 512.0;
        
        if(wInverseFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize, true);
            calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, newBufferSize, false);
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outCompRadix2, true);
            calculator_FFT->radix2_IFFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealRadix2, false);
            calculator_FFT->regular_DFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outCompDFT, true);
            calculator_FFT->regular_IDFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealDFT, false);
            
        }
        else
        {
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize, true);
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealRadix2, true);
            calculator_FFT->regular_DFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealDFT, true);
        }
        
        rememberedBuffer = newBufferSize;
    }
    
    
    
    else if(labelThatHasChanged == &matrixDividerEdit)
    {
        if(wInverseFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDividerEdit.getText().getIntValue());
            calculator_FFT->mixedRadix_IFFT.wSetRadixDivider(matrixDividerEdit.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer, true);
            calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, rememberedBuffer, false);
        }
        else
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDividerEdit.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer, true);
        }
        matrixSizeInfo.setText(matrixDimToString(), dontSendNotification);
    }
    
    refresh();
}





void FFTInterface::updateToggleState        (Button* button, int fftIdentifier)
{
    switch (fftIdentifier)
    {
        case 0:
            setOFF_fft();
            break;
            
        case 1:
            setON_matrixfft();
            break;
            
        case 2:
            setON_radix2fft();
            break;
            
        case 3:
            setON_regular_DFT();
            break;
            
        case 4:
            setInverse_fft();
            break;
            
        case 5:
            setWindowing();
            break;
            
        default:
            return;
    }
}

void FFTInterface::updateToggleZeroPad      (Button* button, int fftIdentifier)
{
    switch (fftIdentifier)
    {
        case 2:
        {
            if(!zeroPadding.getToggleState())
            {
                calculator_FFT->radix2_FFT.setZeroPadding(false);
            }
            else
            {
                calculator_FFT->radix2_FFT.setZeroPadding(true);
            }
            
            if(wInverseFFT.getToggleState())
            {
                calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outCompRadix2, true);
                calculator_FFT->radix2_IFFT.wSettings(wSampleRate, calculator_FFT->radix2_FFT.getBufferSize(), calculator_FFT->outRealRadix2, false);
            }
            else
            {
                calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outRealRadix2, true);
            }
            
            double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
            calculator_FFT->setNewBufSize(tempBuf);
            graphAnalyser->setNewBufSize(tempBuf);
            calculator_FFT->setRadix2BuffSize(rememberedBuffer);
            fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
            zerosPaddingDescript.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                                      rememberedBuffer, tempBuf-rememberedBuffer));
            break;
        }
            
        default:
            break;
    }
}





void FFTInterface::setSampleRate            (double sample_rate)
{
    wSampleRate = sample_rate;
    filterSetLowEnd.setRange(0.0, wSampleRate/1.0, 1.0);
    filterSetLowEnd.setValue(0.0);
    filterSetLowEnd.setSkewFactorFromMidPoint(1000);
    
    filterSetTopEnd.setRange(0.0, wSampleRate/1.0, 1.0);
    filterSetTopEnd.setValue(wSampleRate/1.0);
    filterSetTopEnd.setSkewFactorFromMidPoint(1000);
}





void FFTInterface::setOFF_fft               ()
{
    if(graphAnalyser->isTimerRunning())
        graphAnalyser->stopTimer();
    
    matrixDividerEdit.setVisible(false);
    matrixSizeInfo.setVisible(false);
    matrixDividerEditDescript.setVisible(false);
    selectMatrixFFT.setToggleState(false, NotificationType::dontSendNotification);
    selectRadix2FFT.setToggleState(false, NotificationType::dontSendNotification);

    zeroPadding.setVisible(false);

    zerosPaddingDescript.setVisible(false);
    filterSetTopEnd.setVisible(false);
    filterSetLowEnd.setVisible(false);
    setPhase.setVisible(false);
    wInverseFFT.setVisible(false);
    filtersDescript.setVisible(false);

    calculator_FFT->selectFFT(0);
    repaint();
}

void FFTInterface::setON_matrixfft          ()
{
    if(selectMatrixFFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        
        zerosPaddingDescript.setVisible(false);
        zeroPadding.setVisible(false);
        
        addAndMakeVisible(&wInverseFFT);
        if(wInverseFFT.getToggleState())
            addAndMakeVisible(&setPhase);
        
        addAndMakeVisible(&filterSetLowEnd);
        addAndMakeVisible(&filterSetTopEnd);
        addAndMakeVisible(&filtersDescript);
        filtersDescript.setText("Filters don't impact on sound", dontSendNotification);
        
        addAndMakeVisible(&matrixDividerEdit);
        addAndMakeVisible(&matrixSizeInfo);
        addAndMakeVisible(&matrixDividerEditDescript);

        matrixSizeInfo.setText(matrixDimToString(), dontSendNotification);
        matrixDividerEdit.setText(to_string(calculator_FFT->mixedRadix_FFT.getRadDivider()), dontSendNotification);
        
        filterSetLowEnd.setValue(calculator_FFT->mixedRadix_FFT.getLowEnd());
        filterSetTopEnd.setValue(calculator_FFT->mixedRadix_FFT.getTopEnd());
        setPhase.setValue(calculator_FFT->mixedRadix_IFFT.getPhase());

        
        double tempBuf = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        
        
        calculator_FFT->wOutput = &calculator_FFT->outRealMixed;
        calculator_FFT->resetOutputData();
        calculator_FFT->selectFFT(1);
        graphAnalyser->setLowEndIndex();
        repaint();
        
        if(!graphAnalyser->isTimerRunning())
            graphAnalyser->startTimer(40);
        
        calculator_FFT->dataIsInUse = false;
        calculator_FFT->mixedRadix_FFT.setTopEnd(44100);
    }
}

void FFTInterface::setON_radix2fft          ()
{
    if(selectRadix2FFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        
        matrixDividerEdit.setVisible(false);
        matrixSizeInfo.setVisible(false);
        matrixDividerEditDescript.setVisible(false);
    
        addAndMakeVisible(&wInverseFFT);
        if(wInverseFFT.getToggleState())
            addAndMakeVisible(&setPhase);
        
        addAndMakeVisible(&filterSetLowEnd);
        addAndMakeVisible(&filterSetTopEnd);
        addAndMakeVisible(&filtersDescript);
        filtersDescript.setText("Filters don't impact on sound", dontSendNotification);
        
        addAndMakeVisible(&zeroPadding);
        addAndMakeVisible(&zerosPaddingDescript);
        
        filterSetLowEnd.setValue(calculator_FFT->radix2_FFT.getLowEnd());
        filterSetTopEnd.setValue(calculator_FFT->radix2_FFT.getTopEnd());
        setPhase.setValue(calculator_FFT->radix2_IFFT.getPhase());
        

        double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        zerosPaddingDescript.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                                  rememberedBuffer, tempBuf-rememberedBuffer));
        
        calculator_FFT->wOutput = &calculator_FFT->outRealRadix2;
        calculator_FFT->setRadix2BuffSize(rememberedBuffer);
        calculator_FFT->resetOutputData();
        calculator_FFT->selectFFT(2);
        graphAnalyser->setLowEndIndex();
        repaint();
        
        if(!graphAnalyser->isTimerRunning())
            graphAnalyser->startTimer(40);
        
        calculator_FFT->dataIsInUse = false;
    }
}

void FFTInterface::setON_regular_DFT        ()
{
    if(selectRegDFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        
        matrixDividerEdit.setVisible(false);
        matrixSizeInfo.setVisible(false);
        matrixDividerEditDescript.setVisible(false);
        zerosPaddingDescript.setVisible(false);
        zeroPadding.setVisible(false);
        
        addAndMakeVisible(&wInverseFFT);
        if(wInverseFFT.getToggleState())
            addAndMakeVisible(&setPhase);
        
        addAndMakeVisible(&filterSetLowEnd);
        addAndMakeVisible(&filterSetTopEnd);
        addAndMakeVisible(&filtersDescript);
        filtersDescript.setText("Filters don't impact on sound", dontSendNotification);
        
        filterSetLowEnd.setValue(calculator_FFT->regular_DFT.getLowEnd());
        filterSetTopEnd.setValue(calculator_FFT->regular_DFT.getTopEnd());
        setPhase.setValue(calculator_FFT->regular_IDFT.getPhase());
        
        
        double tempBuf = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        graphAnalyser->setNewBufSize(tempBuf);
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        
        calculator_FFT->wOutput = &calculator_FFT->outRealDFT;
        calculator_FFT->resetOutputData();
        calculator_FFT->selectFFT(3);
        graphAnalyser->setLowEndIndex();
        repaint();
        
        if(!graphAnalyser->isTimerRunning())
            graphAnalyser->startTimer(40);
        
        calculator_FFT->dataIsInUse = false;
    }
}

void FFTInterface::setInverse_fft           ()
{
    calculator_FFT->dataIsInUse = true;
    calculator_FFT->dataIsReadyToFFT = false;
    
    
    if(wInverseFFT.getToggleState())
    {
        addAndMakeVisible(&winHann);
        addAndMakeVisible(&setPhase);
        
        calculator_FFT->isForward = false;
        graphAnalyser->isForward = false;
        
        double tempBuff1 = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, tempBuff1, true);
        calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, tempBuff1, false);
        calculator_FFT->mixedRadix_FFT.setTopEnd(44100);
        calculator_FFT->mixedRadix_IFFT.setTopEnd(44100);


        double tempBuff2 = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outCompRadix2, true);
        calculator_FFT->radix2_IFFT.wSettings(wSampleRate, tempBuff2, calculator_FFT->outRealRadix2, false);
        
        double tempBuff3 = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->regular_DFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outCompDFT, true);
        calculator_FFT->regular_IDFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outRealDFT, false);
    }
    else
    {
        winHann.setVisible(false);
        setPhase.setVisible(false);
        
        calculator_FFT->isForward = true;
        graphAnalyser->isForward = true;
        
        double tempBuff1 = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, tempBuff1, true);
        
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outRealRadix2, true);
        
        double tempBuff3 = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->regular_DFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outRealDFT, true);
    }

    refresh();
}

void FFTInterface::setWindowing             ()
{
    if(winHann.getToggleState())
    {
        calculator_FFT->mixedRadix_IFFT.setWindowing(true);
        calculator_FFT->regular_IDFT.setWindowing(true);
        calculator_FFT->radix2_IFFT.setWindowing(true);
    }
    else
    {
        calculator_FFT->mixedRadix_IFFT.setWindowing(false);
        calculator_FFT->regular_IDFT.setWindowing(false);
        calculator_FFT->radix2_IFFT.setWindowing(false);
    }
}

void FFTInterface::refresh                  ()
{
    if(selectMatrixFFT.getToggleState())
        setON_matrixfft();
    else if(selectRadix2FFT.getToggleState())
        setON_radix2fft();
    else if(selectRegDFT.getToggleState())
        setON_regular_DFT();
    else
        return;
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
