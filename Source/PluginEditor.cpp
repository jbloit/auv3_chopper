/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

Chopper_auv3AudioProcessorEditor::Chopper_auv3AudioProcessorEditor (Chopper_auv3AudioProcessor& p)
: AudioProcessorEditor (&p),
    knobSlider (Slider::LinearHorizontal, Slider::NoTextBox),
    processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    auto bounds = getBounds();
    
    knobSlider.addListener(this);
    knobSlider.setRange(0.0, 1.0);
    addAndMakeVisible (knobSlider);
    
    steppedLenLabel.setBounds(0, 0, 100, 40);
    steppedLenLabel.setText("0", NotificationType::sendNotification);
    addAndMakeVisible(steppedLenLabel);
    
    editModeButton.addListener(this);
    editModeButton.setBounds(120, 0, 40, 40);
    addAndMakeVisible(editModeButton);
    
    p.AudioProcessor::addListener(this);
    
    setSize (600, 200);
    startTimer (100);
}

Chopper_auv3AudioProcessorEditor::~Chopper_auv3AudioProcessorEditor()
{
}

//==============================================================================
void Chopper_auv3AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("It's chopper baby", getLocalBounds(), Justification::centred, 1);
}
#pragma mark -- callbacks
void Chopper_auv3AudioProcessorEditor::resized(){

    Rectangle<int> r = getLocalBounds();
    int guiElementAreaHeight = r.getHeight() / 3;
    
    int margin = guiElementAreaHeight / 4;
    r.reduce (margin, margin);
    
    int buttonHeight = guiElementAreaHeight - margin;
    
    knobSlider.setBounds (r.removeFromTop (guiElementAreaHeight).withSizeKeepingCentre (r.getWidth(), buttonHeight));
}
void Chopper_auv3AudioProcessorEditor::sliderDragStarted (Slider*){
//    processor.editMode = true;
}
void Chopper_auv3AudioProcessorEditor::sliderDragEnded (Slider*){
//    processor.editMode = false;
}
void Chopper_auv3AudioProcessorEditor::sliderValueChanged (Slider*){
    setParameterValue ("knobParam", knobSlider.getValue());
}

void Chopper_auv3AudioProcessorEditor::buttonClicked (Button*){

}

void Chopper_auv3AudioProcessorEditor::audioProcessorParameterChangeGestureBegin(AudioProcessor* p, int parameterIndex){
    printf(" -- audioProcessorParameterChangeGestureBegin, param index %d \n", parameterIndex);
//    processor.editMode = true;
}

void Chopper_auv3AudioProcessorEditor::audioProcessorParameterChangeGestureEnd(AudioProcessor* p, int parameterIndex){
    printf(" ** audioProcessorParameterChangeGestureEnd, param index %d \n", parameterIndex);
//    processor.editMode = false;
}
void Chopper_auv3AudioProcessorEditor::audioProcessorChanged (AudioProcessor* processor){
//    printf(" ** audioProcessorChanged \n");
}

void Chopper_auv3AudioProcessorEditor::audioProcessorParameterChanged (AudioProcessor* processor,
                                     int parameterIndex,
                                     float newValue){
//    printf(" ** audioProcessorParameterChanged %d : %f \n", parameterIndex, newValue);
}
//==============================================================================
void Chopper_auv3AudioProcessorEditor::timerCallback()
{
    knobSlider.setValue (getParameterValue ("knobParam"), NotificationType::dontSendNotification);
    steppedLenLabel.setText(String(processor.steppedLen), NotificationType::dontSendNotification);
    
    bool editMode;
    getParameterValue ("editParam") > 0.0 ?  editMode = true : editMode = false;
    editModeButton.setToggleState(editMode, NotificationType::dontSendNotification);
}

//==============================================================================
AudioProcessorParameter* Chopper_auv3AudioProcessorEditor::getParameter (const String& paramId)
{
    if (AudioProcessor* processor = getAudioProcessor())
    {
        const OwnedArray<AudioProcessorParameter>& params = processor->getParameters();
        
        for (int i = 0; i < params.size(); ++i)
        {
            if (AudioProcessorParameterWithID* param = dynamic_cast<AudioProcessorParameterWithID*> (params[i]))
            {
                if (param->paramID == paramId)
                    return param;
            }
        }
    }
    
    return nullptr;
}

//==============================================================================
float Chopper_auv3AudioProcessorEditor::getParameterValue (const String& paramId)
{
    if (AudioProcessorParameter* param = getParameter (paramId))
        return param->getValue();
    return 0.0f;
}


void Chopper_auv3AudioProcessorEditor::setParameterValue (const String& paramId, float value)
{
    if (AudioProcessorParameter* param = getParameter (paramId))
        param->setValueNotifyingHost (value);
}
