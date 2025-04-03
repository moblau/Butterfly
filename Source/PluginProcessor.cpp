/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ButterflyAudioProcessor::ButterflyAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameters())
{

    DBG("TEST");
    // ... repeat for 4 voices
    synth.clearVoices();
    for (int i = 0; i < 4; ++i) // 8-voice polyphony
        synth.addVoice(new FMVoice());

    synth.clearSounds();
    synth.addSound(new FMSound());
    gainProcessor.setRampDurationSeconds(0.2);
}

ButterflyAudioProcessor::~ButterflyAudioProcessor()
{
}

//==============================================================================
const juce::String ButterflyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ButterflyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ButterflyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ButterflyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ButterflyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ButterflyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ButterflyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ButterflyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ButterflyAudioProcessor::getProgramName (int index)
{
    return {};
}

void ButterflyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ButterflyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void ButterflyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ButterflyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void ButterflyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear(); // Clear buffer before processing
    
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<FMVoice*>(synth.getVoice(i)))
        {
            switch (i)
            {
                case 0:
                    voice->setPan(*apvts.getRawParameterValue("PAN1"));
                    voice->setModulationRatio(
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM1")),
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN1")));
                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX1"));
                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM1")));
                    voice->setDetune(*apvts.getRawParameterValue("DETUNE1"));
                    break;
                case 1:
                    voice->setPan(*apvts.getRawParameterValue("PAN2"));
                    voice->setModulationRatio(
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM2")),
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN2")));
                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX2"));
                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM2")));
                    voice->setDetune(*apvts.getRawParameterValue("DETUNE2"));
                    break;
                case 2:
                    voice->setPan(*apvts.getRawParameterValue("PAN3"));
                    voice->setModulationRatio(
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM3")),
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN3")));
                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX3"));
                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM3")));
                    voice->setDetune(*apvts.getRawParameterValue("DETUNE3"));
                    break;
                case 3:
                    voice->setPan(*apvts.getRawParameterValue("PAN4"));
                    voice->setModulationRatio(
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM4")),
                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN4")));
                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX4"));
                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM4")));
                    voice->setDetune(*apvts.getRawParameterValue("DETUNE4"));
                    break;
                default:
                    break;
            }
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    if (auto* editor = dynamic_cast<ButterflyAudioProcessorEditor*>(getActiveEditor()))
    {
        int currentStep = editor->getCurrentStep();
        
        
        // Get the step value from the parameter value tree (APVTS)
        float stepValue = *apvts.getRawParameterValue("step" + juce::String(currentStep));  // Access the value of the current step
        
        gainProcessor.setGainLinear(stepValue);
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);  // Create a process context
        gainProcessor.process(context);
    }
    
    
    
}

//==============================================================================
bool ButterflyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ButterflyAudioProcessor::createEditor()
{
    return new ButterflyAudioProcessorEditor (*this);
}

//==============================================================================
void ButterflyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ButterflyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ButterflyAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout ButterflyAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // Panning Parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN1", 1), "Pan 1", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN2", 1), "Pan 2", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN3", 1), "Pan 3", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN4", 1), "Pan 4", 0.0f, 1.0f, 0.5f));

    // Detune Parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE1", 1), "Detune 1", -0.5f, 0.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE2", 1), "Detune 2", -0.5f, 0.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE3", 1), "Detune 3", -0.5f, 0.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE4", 1), "Detune 4", -0.5f, 0.5f, 0.0f));

    // Modulation Ratio Numerator and Denominator (1 to 16)
    for (int i = 1; i <= 4; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_RATIO_NUM" + std::to_string(i), 1),
            "Mod Ratio Numerator " + std::to_string(i),
            1, 16, 1));

        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_RATIO_DEN" + std::to_string(i), 1),
            "Mod Ratio Denominator " + std::to_string(i),
            1, 16, 1));
    }

    // Modulation Index
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_INDEX1", 1), "Mod Index 1", 0.0f, 50.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_INDEX2", 1), "Mod Index 2", 0.0f, 50.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_INDEX3", 1), "Mod Index 3", 0.0f, 50.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_INDEX4", 1), "Mod Index 4", 0.0f, 50.0f, 1.0f));

    // Modulator Waveform Selectors
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("MOD_WAVEFORM1", 1), "Mod Waveform 1",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("MOD_WAVEFORM2", 1), "Mod Waveform 2",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("MOD_WAVEFORM3", 1), "Mod Waveform 3",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("MOD_WAVEFORM4", 1), "Mod Waveform 4",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    // Step Sequencer Parameters (8 steps)
    for (int i = 0; i < 8; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("step" + juce::String(i), 1),
            "Step " + juce::String(i + 1),
            0.0f, 1.0f, 1.0f));
    }

    return { parameters.begin(), parameters.end() };
}
