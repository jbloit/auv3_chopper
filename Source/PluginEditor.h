/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/

class Chopper_auv3AudioProcessorEditor  : public AudioProcessorEditor,
                                         public Slider::Listener,
                                         public Button::Listener,
                                         public AudioProcessorListener,
                                         private Timer
{
public:
    Chopper_auv3AudioProcessorEditor (Chopper_auv3AudioProcessor&);
    ~Chopper_auv3AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
private:
    Slider knobSlider;
    ToggleButton editModeButton;
    
    Label steppedLenLabel;
    void sliderValueChanged (Slider*) override;
    void sliderDragStarted (Slider*) override;
    void sliderDragEnded (Slider*) override;
    void buttonClicked(Button*) override;
    void audioProcessorParameterChangeGestureBegin(AudioProcessor* processor, int parameterIndex) override;
    void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex) override;
    void audioProcessorChanged (AudioProcessor* processor) override;
    void audioProcessorParameterChanged (AudioProcessor* processor,
                                                 int parameterIndex,
                                                 float newValue) override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Chopper_auv3AudioProcessor& processor;
    void timerCallback() override;
    AudioProcessorParameter* getParameter (const String& paramId);
    float getParameterValue (const String& paramId);
    void setParameterValue (const String& paramId, float value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Chopper_auv3AudioProcessorEditor)
};
