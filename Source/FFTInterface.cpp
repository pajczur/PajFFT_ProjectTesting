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
    remembered_fRangeBuffer = 512;
    
    addAndMakeVisible(&fftBufSizeEdit);
    fftBufSizeEdit.setEditable(true);
    fftBufSizeEdit.setText("512", dontSendNotification);
    fftBufSizeEdit.setJustificationType(Justification::centred);
    fftBufSizeEdit.addListener(this);
    
    addAndMakeVisible(&fftBufSize);
    fftBufSize.attachToComponent(&fftBufSizeEdit, true);
    fftBufSize.setText("Set buffer size: ", dontSendNotification);
    fftBufSize.setJustificationType(Justification::centredRight);
    
    
    
    addAndMakeVisible(&selectMatrixFFT);
    selectMatrixFFT.setRadioGroupId(waveSelectorButtons);
    selectMatrixFFT.onClick = [this] { updateToggleState(&selectMatrixFFT, matrixFFT_ID); };
    selectMatrixFFT.setButtonText("Matrix FFT");
    
    addAndMakeVisible(&selectRadix2FFT);
    selectRadix2FFT.setRadioGroupId(waveSelectorButtons);
    selectRadix2FFT.onClick = [this] { updateToggleState(&selectRadix2FFT, radix2FFT_ID); };
    selectRadix2FFT.setButtonText("Radix-2 FFT");
    
    addAndMakeVisible(&selectRegDFT);
    selectRegDFT.setRadioGroupId(waveSelectorButtons);
    selectRegDFT.onClick = [this] { updateToggleState(&selectRegDFT, regDFT_ID); };
    selectRegDFT.setButtonText("Regular DFT");
    
    
    wInverseFFT.onClick = [this] { updateToggleState(&wInverseFFT, wInverse_ID); };
    wInverseFFT.setButtonText("INVERSE");

    addAndMakeVisible(&turnOFF);
    turnOFF.setButtonText("OFF");
    turnOFF.setRadioGroupId(waveSelectorButtons);
    turnOFF.onClick = [this] { updateToggleState(&turnOFF, turnOFF_ID); };
    
    zeroPaddingRad2.setToggleState(true, dontSendNotification);
    zeroPaddingRad2.onClick = [this] { updateToggleZeroPad(&zeroPaddingRad2, radix2FFT_ID); };
    zeroPaddingRad2.setButtonText("Zeros padding");

    zerosInfo.setReadOnly(true);
    zerosInfo.setMultiLine(true);
    zerosInfo.setCaretVisible(false);

    zerosInfo.setText("Uses all 512\nof samples");
    
    setLowEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    setLowEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    setLowEnd.setTextValueSuffix(" Hz");
    setLowEnd.addListener(this);
    setTopEnd.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    setTopEnd.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    setTopEnd.setTextValueSuffix(" Hz");
    setTopEnd.addListener(this);
    
    wPhase.setText("Phase", dontSendNotification);
    wPhase.setJustificationType(Justification::centredBottom);
    wPhase.attachToComponent(&setPhase, false);
    setPhase.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setPhase.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    setPhase.setTextValueSuffix(" *i^x");
    setPhase.addListener(this);
    setPhase.setRange(0.0, 8.0, 0.01);
    setPhase.setValue(0.0);
    
    winHann.setButtonText("Hann Window");
    winHann.onClick = [this] { updateToggleState(&winHann, winHann_ID); };
    
    lowtopInfo.setEditable(false);
    lowtopInfo.setJustificationType(Justification::centred);
    
    matrixSize.setEditable(false);
    matrixSize.setJustificationType(Justification::topLeft);
    matrixDivider.setEditable(true);
    matrixDivider.setJustificationType(Justification::centredLeft);
    matrixDivider.addListener(this);
    divideClickInfo.setEditable(false);
    divideClickInfo.setText("Divider (click to change)", dontSendNotification);
    divideClickInfo.setJustificationType(Justification::centredLeft);
}

FFTInterface::~FFTInterface()
{
}

void FFTInterface::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    
    bufferBox.setSize(80, 25);
    g.drawRect(bufferBox);
    
    if(selectMatrixFFT.getToggleState())
    {
        g.drawRect(matDiviBox);
        g.drawRect(matSizeBox);
    }
}

void FFTInterface::resized()
{
    bufferBox.setBounds(100, 3, 80, 25);
    fftBufSizeEdit.setBounds(100, 3, 80, 25);


    zeroPaddingRad2.setBounds(200, 5, 100, 20);
    zerosInfo.setBounds(200, 28, 160, 35);

    setLowEnd.setBounds(200, 87, 70, 50);
    setTopEnd.setBounds(290, 87, 70, 50);
    
    setPhase.setBounds(390, (getHeight()/2)-50, 90, 90);
    winHann.setBounds(410, getHeight()-33, 55, 25);

    lowtopInfo.setBounds(185, 135, 210, 25);

    matDiviBox.setBounds(200, 3, 30, 25);
    divideClickInfo.setBounds(233, 3, 131, 25);
    matrixDivider.setBounds(201, 3, 28, 28);
    matSizeBox.setBounds(200, 30, 160, 50);
    matrixSize.setBounds(200, 30, 160, 50);
    
    int topMargin = 35;
    int elementWidth = 80;
    int elementHeight = 30;
    int spaceBetween = 30;
    int leftMargin = 65;
    
    selectMatrixFFT.setBounds      (leftMargin,                  topMargin,   elementWidth, elementHeight);
    selectRadix2FFT.setBounds      (leftMargin, topMargin+1*(spaceBetween),   elementWidth, elementHeight);
    selectRegDFT.setBounds         (leftMargin, topMargin+2*(spaceBetween),   elementWidth, elementHeight);
    turnOFF.setBounds              (10, 40,   50, 110);
    
    wInverseFFT.setBounds          (leftMargin+ 20, topMargin+3*(spaceBetween), elementWidth, elementHeight);
    
    selectMatrixFFT.changeWidthToFitText();
    selectRadix2FFT.changeWidthToFitText();
    selectRegDFT.changeWidthToFitText();
}



void FFTInterface::labelTextChanged (Label *labelThatHasChanged)
{
    if(labelThatHasChanged == &fftBufSizeEdit)
    {
        calculator_FFT->dataIsInUse = true;
        calculator_FFT->dataIsReadyToFFT = false;
        calculator_FFT->stopTimer();
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
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outCompMixed, true);
            calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealMixed_1, false);
            calculator_FFT->dataIsAfterFFT = false;
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outCompRadix2, true);
            calculator_FFT->radix2_IFFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealRadix2_1, false);
            calculator_FFT->regular_DFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outCompDFT, true);
            calculator_FFT->regular_IDFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealDFT_1, false);
            
        }
        else
        {
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealMixed_1, true);
            calculator_FFT->radix2_FFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealRadix2_1, true);
            calculator_FFT->regular_DFT.wSettings(wSampleRate, newBufferSize, calculator_FFT->outRealDFT_1, true);
        }
        
        rememberedBuffer = newBufferSize;
        if(selectMatrixFFT.getToggleState())
        {
            double tempBuf = calculator_FFT->mixedRadix_FFT.getBufferSize();
            calculator_FFT->setNewBufSize(tempBuf);
            calculator_FFT->resetOutputData();
            fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
            matrixSize.setText(matrixDim(), dontSendNotification);
//                graphAnalyser->selectFFT(1);
            calculator_FFT->setLowEndIndex();
            calculator_FFT->dataIsInUse = false;
            calculator_FFT->startTimer(ceil((tempBuf/wSampleRate)*100));
        }
        if(selectRadix2FFT.getToggleState())
        {
            double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
            calculator_FFT->setNewBufSize(tempBuf);
            calculator_FFT->setRadix2BuffSize(rememberedBuffer);
            calculator_FFT->resetOutputData();
            fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
            zerosInfo.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                           rememberedBuffer, tempBuf-rememberedBuffer));
//                graphAnalyser->selectFFT(2);
            calculator_FFT->setLowEndIndex();
            calculator_FFT->dataIsInUse = false;
            calculator_FFT->startTimer(ceil((tempBuf/wSampleRate)*1000));
        }
        if(selectRegDFT.getToggleState())
        {
            double tempBuf = calculator_FFT->regular_DFT.getBufferSize();
            calculator_FFT->setNewBufSize(tempBuf);
            calculator_FFT->resetOutputData();
            fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);

//                graphAnalyser->selectFFT(3);
            calculator_FFT->setLowEndIndex();
            calculator_FFT->dataIsInUse = false;
            calculator_FFT->startTimer(2000);
        }
    }
    else if(labelThatHasChanged == &matrixDivider)
    {
        if(wInverseFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDivider.getText().getIntValue());
            calculator_FFT->mixedRadix_IFFT.wSetRadixDivider(matrixDivider.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outCompMixed, true);
            calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outRealMixed_1, false);
        }
        else
        {
            calculator_FFT->mixedRadix_FFT.wSetRadixDivider(matrixDivider.getText().getIntValue());
            calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outRealMixed_1, true);
        }
        matrixSize.setText(matrixDim(), dontSendNotification);
    }
}

void FFTInterface::wSettings(CalculateDTFT &graph, OscInterface &osPan)
{
    calculator_FFT = &graph;
    oscPan = &osPan;
}

void FFTInterface::setSampleRate(double sample_rate)
{
    wSampleRate = sample_rate;
    setLowEnd.setRange(0.0, wSampleRate/2.0, 1.0);
    setLowEnd.setValue(0.0);
    setLowEnd.setSkewFactorFromMidPoint(1000);
    
    setTopEnd.setRange(0.0, wSampleRate/2.0, 1.0);
    setTopEnd.setValue(wSampleRate/2.0);
    setTopEnd.setSkewFactorFromMidPoint(1000);
}


void FFTInterface::updateToggleState(Button* button, int fftIdentifier)
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
            wAudioApplication->repaint();
            break;
            
        case 5:
            setWindowing();
            break;
            
        default:
            return;
    }
}


void FFTInterface::stopEverything()
{
    calculator_FFT->selectFFT(0);
    calculator_FFT->stopTimer();
    calculator_FFT->fftGraph.clear();
    calculator_FFT->repaint();
    oscPan->updateToggleState(&oscPan->wMuteButton, oscPan->mutIdentifier);
}


void FFTInterface::updateToggleZeroPad(Button* button, int fftIdentifier)
{
    switch (fftIdentifier)
    {
        case 2:
        {
            if(!zeroPaddingRad2.getToggleState())
            {
                calculator_FFT->radix2_FFT.setZeroPadding(false);
//                graphAnalyser->radix2_IFFT.setZeroPadding(false);
            }
            else
            {
                calculator_FFT->radix2_FFT.setZeroPadding(true);
//                graphAnalyser->radix2_IFFT.setZeroPadding(false);
            }
            
            if(wInverseFFT.getToggleState())
            {
                calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outCompRadix2, true);
                calculator_FFT->radix2_IFFT.wSettings(wSampleRate, calculator_FFT->radix2_FFT.getBufferSize(), calculator_FFT->outRealRadix2_1, false);
            }
            else
            {
                calculator_FFT->radix2_FFT.wSettings(wSampleRate, rememberedBuffer, calculator_FFT->outRealRadix2_1, true);
            }
            
            double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
            calculator_FFT->setNewBufSize(tempBuf);
            calculator_FFT->setRadix2BuffSize(rememberedBuffer);
            fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
            zerosInfo.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                           rememberedBuffer, tempBuf-rememberedBuffer));
            break;
        }
            
        default:
            break;
    }
}

string FFTInterface::setZerosInfo(int use, int bufSize, int zero)
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


void FFTInterface::setOFF_fft()
{
    matrixDivider.setVisible(false);
    matrixSize.setVisible(false);
    divideClickInfo.setVisible(false);
    selectMatrixFFT.setToggleState(false, NotificationType::dontSendNotification);
    selectRadix2FFT.setToggleState(false, NotificationType::dontSendNotification);

    zeroPaddingRad2.setVisible(false);

    zerosInfo.setVisible(false);
    setTopEnd.setVisible(false);
    setLowEnd.setVisible(false);
    setPhase.setVisible(false);
    wInverseFFT.setVisible(false);
    lowtopInfo.setVisible(false);

    calculator_FFT->selectFFT(0);
    if(calculator_FFT->isTimerRunning())
    {
        calculator_FFT->stopTimer();
        calculator_FFT->fftGraph.clear();
        calculator_FFT->repaint();
    }
    repaint();
}

void FFTInterface::setON_matrixfft()
{
    if(selectMatrixFFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        calculator_FFT->stopTimer();
        
        addAndMakeVisible(&wInverseFFT);
        addAndMakeVisible(&matrixDivider);
        matrixDivider.setText(to_string(calculator_FFT->mixedRadix_FFT.getRadDivider()), dontSendNotification);
        addAndMakeVisible(&matrixSize);
        matrixSize.setText(matrixDim(), dontSendNotification);
        addAndMakeVisible(&divideClickInfo);

        addAndMakeVisible(&setLowEnd);
        addAndMakeVisible(&setTopEnd);
        addAndMakeVisible(&lowtopInfo);
        lowtopInfo.setText("Filters don't impact on sound", dontSendNotification);
        zeroPaddingRad2.setVisible(false);
        
        setLowEnd.setValue(calculator_FFT->mixedRadix_FFT.getLowEnd());
        setTopEnd.setValue(calculator_FFT->mixedRadix_FFT.getTopEnd());
        setPhase.setValue(calculator_FFT->mixedRadix_IFFT.getPhase());

        if(wInverseFFT.getToggleState())
        addAndMakeVisible(&setPhase);
        
        double tempBuf = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        calculator_FFT->wOutput1 = &calculator_FFT->outRealMixed_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealMixed_2;
        calculator_FFT->resetOutputData();
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        calculator_FFT->selectFFT(1);
        zerosInfo.setVisible(false);
        repaint();
        calculator_FFT->setLowEndIndex();
        calculator_FFT->dataIsInUse = false;
        calculator_FFT->startTimer(ceil(1000*(tempBuf/wSampleRate)));
    }
}

void FFTInterface::setON_radix2fft()
{
    if(selectRadix2FFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        calculator_FFT->stopTimer();

        matrixDivider.setVisible(false);
        matrixSize.setVisible(false);
        divideClickInfo.setVisible(false);
        
        addAndMakeVisible(&wInverseFFT);
        addAndMakeVisible(&setLowEnd);
        addAndMakeVisible(&setTopEnd);
        addAndMakeVisible(&lowtopInfo);
        lowtopInfo.setText("Filters don't impact on sound", dontSendNotification);
        addAndMakeVisible(&zeroPaddingRad2);
        
        setLowEnd.setValue(calculator_FFT->radix2_FFT.getLowEnd());
        setTopEnd.setValue(calculator_FFT->radix2_FFT.getTopEnd());
        setPhase.setValue(calculator_FFT->radix2_IFFT.getPhase());
        
        if(wInverseFFT.getToggleState())
        addAndMakeVisible(&setPhase);

        double tempBuf = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        calculator_FFT->setRadix2BuffSize(rememberedBuffer);
        calculator_FFT->wOutput1 = &calculator_FFT->outRealRadix2_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealRadix2_2;
        calculator_FFT->resetOutputData();
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        calculator_FFT->selectFFT(2);
        zerosInfo.setText(setZerosInfo(rememberedBuffer>tempBuf?tempBuf:rememberedBuffer,
                                       rememberedBuffer, tempBuf-rememberedBuffer));
        addAndMakeVisible(&zerosInfo);
        
        repaint();
        calculator_FFT->setLowEndIndex();
        calculator_FFT->dataIsInUse = false;
        calculator_FFT->startTimer(ceil(1000*(tempBuf/wSampleRate)));
    }
}

void FFTInterface::setON_regular_DFT()
{
    if(selectRegDFT.getToggleState())
    {
        calculator_FFT->selectFFT(0);
        calculator_FFT->stopTimer();
        
        matrixDivider.setVisible(false);
        matrixSize.setVisible(false);
        divideClickInfo.setVisible(false);
        
        addAndMakeVisible(&wInverseFFT);
        addAndMakeVisible(&setLowEnd);
        addAndMakeVisible(&setTopEnd);
        addAndMakeVisible(&lowtopInfo);
        lowtopInfo.setText("Filters don't impact on sound", dontSendNotification);
        zeroPaddingRad2.setVisible(false);
        
        setLowEnd.setValue(calculator_FFT->regular_DFT.getLowEnd());
        setTopEnd.setValue(calculator_FFT->regular_DFT.getTopEnd());
        setPhase.setValue(calculator_FFT->regular_IDFT.getPhase());
        
        if(wInverseFFT.getToggleState())
        addAndMakeVisible(&setPhase);
        
        double tempBuf = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->setNewBufSize(tempBuf);
        calculator_FFT->wOutput1 = &calculator_FFT->outRealDFT_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealDFT_2;
        calculator_FFT->resetOutputData();
        calculator_FFT->fftGraph.clear();
        calculator_FFT->repaint();
        fftBufSizeEdit.setText(to_string((int)tempBuf), dontSendNotification);
        calculator_FFT->selectFFT(3);
        zerosInfo.setVisible(false);
        
        repaint();
        calculator_FFT->setLowEndIndex();
        calculator_FFT->dataIsInUse = false;
        calculator_FFT->startTimer(2000);
    }
}

void FFTInterface::setInverse_fft()
{
    calculator_FFT->dataIsInUse = true;
    calculator_FFT->dataIsReadyToFFT = false;
    calculator_FFT->stopTimer();
    
    
    if(wInverseFFT.getToggleState())
    {
        addAndMakeVisible(&winHann);
        addAndMakeVisible(&setPhase);
        calculator_FFT->isForward = false;
        double tempBuff1 = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, tempBuff1, calculator_FFT->outCompMixed, true);
        calculator_FFT->mixedRadix_IFFT.wSettings(wSampleRate, tempBuff1, calculator_FFT->outRealMixed_1, false);

        double tempBuff2 = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, tempBuff2, calculator_FFT->outCompRadix2, true);
        calculator_FFT->radix2_IFFT.wSettings(wSampleRate, tempBuff2, calculator_FFT->outRealRadix2_1, false);
        
        double tempBuff3 = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->regular_DFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outCompDFT, true);
        calculator_FFT->regular_IDFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outRealDFT_1, false);
    }
    else
    {
        winHann.setVisible(false);
        setPhase.setVisible(false);
        calculator_FFT->isForward = true;
        double tempBuff1 = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->mixedRadix_FFT.wSettings(wSampleRate, tempBuff1, calculator_FFT->outRealMixed_1, true);
        
        double tempBuff2 = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->radix2_FFT.wSettings(wSampleRate, tempBuff2, calculator_FFT->outRealRadix2_1, true);
        
        double tempBuff3 = calculator_FFT->regular_DFT.getBufferSize();
        calculator_FFT->regular_DFT.wSettings(wSampleRate, tempBuff3, calculator_FFT->outRealDFT_1, true);
    }

    if(selectMatrixFFT.getToggleState())
    {
        calculator_FFT->resetOutputData();
        calculator_FFT->wOutput1 = &calculator_FFT->outRealMixed_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealMixed_2;
        calculator_FFT->dataIsInUse = false;
        double tempBuff1 = calculator_FFT->mixedRadix_FFT.getBufferSize();
        calculator_FFT->startTimer(ceil((tempBuff1/wSampleRate)*1000));
    }
    else if(selectRadix2FFT.getToggleState())
    {
        calculator_FFT->resetOutputData();
        calculator_FFT->wOutput1 = &calculator_FFT->outRealRadix2_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealRadix2_2;
        calculator_FFT->dataIsInUse = false;
        double tempBuff1 = calculator_FFT->radix2_FFT.getBufferSize();
        calculator_FFT->startTimer(ceil((tempBuff1/wSampleRate)*1000));
    }
    else if(selectRegDFT.getToggleState())
    {
        calculator_FFT->resetOutputData();
        calculator_FFT->wOutput1 = &calculator_FFT->outRealDFT_1;
        calculator_FFT->wOutput2 = &calculator_FFT->outRealDFT_2;
        calculator_FFT->dataIsInUse = false;
//        double tempBuff1 = graphAnalyser->regular_DFT.getBufferSize();
        calculator_FFT->startTimer(2000);
    }
}

void FFTInterface::setWindowing()
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



void FFTInterface::sliderValueChanged(Slider *slider)
{
    if(slider == &setLowEnd)
    {
        if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_FFT.setLowEnd(setLowEnd.getValue());
            if(setLowEnd.getValue()+11.0 > setTopEnd.getValue())
            {
                setTopEnd.setValue(setLowEnd.getValue()+10.0);
                calculator_FFT->radix2_FFT.setTopEnd(setTopEnd.getValue());
            }
            if(round(setLowEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(setLowEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput1->at(k-1) = 0.0f;
                    calculator_FFT->wOutput2->at(k-1) = 0.0f;
                }
            }
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_DFT.setLowEnd(setLowEnd.getValue());
            if(setLowEnd.getValue()+11.0 > setTopEnd.getValue())
            {
                setTopEnd.setValue(setLowEnd.getValue()+10.0);
                calculator_FFT->regular_DFT.setTopEnd(setTopEnd.getValue());
            }
            if(round(setLowEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(setLowEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput1->at(k-1) = 0.0f;
                    calculator_FFT->wOutput2->at(k-1) = 0.0f;
                }
            }
        }
        else if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.setLowEnd(setLowEnd.getValue());
            if(setLowEnd.getValue()+11.0 > setTopEnd.getValue())
            {
                setTopEnd.setValue(setLowEnd.getValue()+10.0);
                calculator_FFT->mixedRadix_FFT.setTopEnd(setTopEnd.getValue());
            }
            if(round(setLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate))>=0)
            {
                int tempIndex = round(setLowEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
                
                for(int k=tempIndex; k>0; k--)
                {
                    calculator_FFT->wOutput1->at(k-1) = 0.0f;
                    calculator_FFT->wOutput2->at(k-1) = 0.0f;
                }
            }
        }
        else
        {
            return;
        }
    }
    
    if(slider == &setTopEnd)
    {
        if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_FFT.setTopEnd(setTopEnd.getValue());
            if(setTopEnd.getValue()-11.0 < setLowEnd.getValue())
            {
                setLowEnd.setValue(setTopEnd.getValue()-10.0);
                calculator_FFT->radix2_FFT.setLowEnd(setLowEnd.getValue());
            }
            if(round(setTopEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(setTopEnd.getValue()*(calculator_FFT->radix2_FFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->radix2_FFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput1->at(k) = 0.0f;
                    calculator_FFT->wOutput2->at(k) = 0.0f;
                }
            }
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_DFT.setTopEnd(setTopEnd.getValue());
            if(setTopEnd.getValue()-11.0 < setLowEnd.getValue())
            {
                setLowEnd.setValue(setTopEnd.getValue()-10.0);
                calculator_FFT->regular_DFT.setLowEnd(setLowEnd.getValue());
            }
            if(round(setTopEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(setTopEnd.getValue()*(calculator_FFT->regular_DFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->regular_DFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput1->at(k) = 0.0f;
                    calculator_FFT->wOutput2->at(k) = 0.0f;
                }
            }
        }
        else if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_FFT.setTopEnd(setTopEnd.getValue());
            if(setTopEnd.getValue()-11.0 < setLowEnd.getValue())
            {
                setLowEnd.setValue(setTopEnd.getValue()-10.0);
                calculator_FFT->mixedRadix_FFT.setLowEnd(setLowEnd.getValue());
            }
            if(round(setTopEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate))<=(wSampleRate/2.0))
            {
                int tempIndex = round(setTopEnd.getValue()*(calculator_FFT->mixedRadix_FFT.getBufferSize()/wSampleRate));
                for(int k=tempIndex; k<(calculator_FFT->mixedRadix_FFT.getBufferSize()/2.0); k++)
                {
                    calculator_FFT->wOutput1->at(k) = 0.0f;
                    calculator_FFT->wOutput2->at(k) = 0.0f;
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
        if(selectRadix2FFT.getToggleState())
        {
            calculator_FFT->radix2_IFFT.setPhase(setPhase.getValue());
        }
        else if(selectRegDFT.getToggleState())
        {
            calculator_FFT->regular_IDFT.setPhase(setPhase.getValue());
        }
        else if(selectMatrixFFT.getToggleState())
        {
            calculator_FFT->mixedRadix_IFFT.setPhase(setPhase.getValue());
        }
    }
}

string FFTInterface::lEndText(int lEnd)
{
    int low = lEnd;
    string sufixHz = " Hz";
    return to_string(low) + sufixHz;
}

string FFTInterface::tEndText(int tEnd)
{
    int top = tEnd;
    string sufixHz = " Hz";
    return to_string(top) + sufixHz;
}

string FFTInterface::matrixDim()
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
