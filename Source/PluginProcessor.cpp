/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Chopper_auv3AudioProcessor::Chopper_auv3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  ),
m_CurrentBufferSize(0)

#endif
{
    
    // use a default samplerate and vector size here, reset it later
    m_C74PluginState = (CommonState *)GenChopper::create(44100, 64);
    GenChopper::reset(m_C74PluginState);
    
    m_InputBuffers = new t_sample *[GenChopper::num_inputs()];
    m_OutputBuffers = new t_sample *[GenChopper::num_outputs()];
    
    for (int i = 0; i < GenChopper::num_inputs(); i++) {
        m_InputBuffers[i] = NULL;
    }
    for (int i = 0; i < GenChopper::num_outputs(); i++) {
        m_OutputBuffers[i] = NULL;
    }
    
    // ---------------------------- Parameters
    addParameter (knobParam = new AudioParameterFloat ("knobParam", "Knob", 0.0f, 1.0f, 0.5f));
    addParameter (editParam = new AudioParameterBool("editParam", "Edit", false, "Edit label"));
    knobSteps.resize(maxNbSteps);
}

Chopper_auv3AudioProcessor::~Chopper_auv3AudioProcessor()
{
    GenChopper::destroy(m_C74PluginState);
}

//==============================================================================
#pragma mark ---
const String Chopper_auv3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Chopper_auv3AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Chopper_auv3AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Chopper_auv3AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Chopper_auv3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Chopper_auv3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int Chopper_auv3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Chopper_auv3AudioProcessor::setCurrentProgram (int index){

}

const String Chopper_auv3AudioProcessor::getProgramName (int index)
{
    return {};
}

void Chopper_auv3AudioProcessor::changeProgramName (int index, const String& newName)
{
}


//==============================================================================
#pragma mark ---
void Chopper_auv3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Chopper_auv3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Chopper_auv3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void Chopper_auv3AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    
    currentKnobValue = knobParam->get();
//    (knobParam->get() > 0.0) ? editMode = false : editMode = true;
    
    // detect mode change
    if (prevEditMode != editParam->get()){
        if (editParam->get()){
            // reset step sequence
            stepIndex = 0;
            steppedLen = 0;
        }
    }
    prevEditMode = editParam->get();
    
    // ------------------ MIDI processing
    int time;
    MidiMessage m;
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            knobValueChangesCount = 0;
            
            if (editParam->get()){
                // Record steps
                knobSteps.set(stepIndex, knobParam->get());
                stepIndex += 1;
                stepIndex = stepIndex % maxNbSteps;
                steppedLen = stepIndex;
            }
            else {
                // Step through steps, recall
                if (steppedLen > 0){
                    stepIndex += 1;
                    stepIndex = stepIndex % steppedLen;
                    knobParam->setValueNotifyingHost(knobSteps[stepIndex]);
                }
            }
        }
    }

    
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // ------------------- GEN code starts here
    assureBufferSize(buffer.getNumSamples());
    
    // fill input buffers
    for (int i = 0; i < GenChopper::num_inputs(); i++) {
        if (i < totalNumInputChannels) {
            
            for (int j = 0; j < m_CurrentBufferSize; j++) {
                m_InputBuffers[i][j] = buffer.getReadPointer(i)[j];
            }
        } else {
            memset(m_InputBuffers[i], 0, m_CurrentBufferSize *  sizeof(double));
        }
    }
    
    currentKnobValue = knobParam->get();

    //WARNING: MAKE SURE YOUR PARAM INDEX IS CORRECT
    GenChopper::setparameter(m_C74PluginState, 3, currentKnobValue, NULL);
    
    // process audio
    GenChopper::perform(m_C74PluginState,
                       m_InputBuffers,
                       GenChopper::num_inputs(),
                       m_OutputBuffers,
                       GenChopper::num_outputs(),
                       buffer.getNumSamples());
    
    // fill output buffers
    for (int i = 0; i < totalNumOutputChannels; i++) {
        if (i < GenChopper::num_outputs()) {
            for (int j = 0; j < buffer.getNumSamples(); j++) {
                buffer.getWritePointer(i)[j] = m_OutputBuffers[i][j];
            }
        } else {
            buffer.clear (i, 0, buffer.getNumSamples());
        }
    }
}

//==============================================================================
bool Chopper_auv3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Chopper_auv3AudioProcessor::createEditor()
{
    return new Chopper_auv3AudioProcessorEditor (*this);
}

//==============================================================================
void Chopper_auv3AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Chopper_auv3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Chopper_auv3AudioProcessor();
}

//==============================================================================
#pragma mark -- helpers


//==============================================================================
#pragma mark -- C74 added methods

void Chopper_auv3AudioProcessor::assureBufferSize(long bufferSize)
{
    if (bufferSize > m_CurrentBufferSize) {
        for (int i = 0; i < GenChopper::num_inputs(); i++) {
            if (m_InputBuffers[i]) delete m_InputBuffers[i];
            m_InputBuffers[i] = new t_sample[bufferSize];
        }
        for (int i = 0; i < GenChopper::num_outputs(); i++) {
            if (m_OutputBuffers[i]) delete m_OutputBuffers[i];
            m_OutputBuffers[i] = new t_sample[bufferSize];
        }
        
        m_CurrentBufferSize = bufferSize;
    }
}
