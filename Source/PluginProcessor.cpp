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
apvts(*this, nullptr, "PARAMETERS", createParameters()), fxChainProcessor(apvts)
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
    fxChainProcessor.prepare(sampleRate, samplesPerBlock);
//    alienWah.prepare(sampleRate, samplesPerBlock, sampleRate); // Allow a reasonable max delay
//    alienWah.setParameters(0.6f, 0.0f, 0.5f, 20);
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
    int currentStep = 0;
    if (auto* playhead = getPlayHead()){
        juce::AudioPlayHead::CurrentPositionInfo info;
        if ( playhead->getCurrentPosition(info)){
            int stepCount = static_cast<int>(*apvts.getRawParameterValue("STEP_COUNT"));
            
            int rateIndex = static_cast<int>(*apvts.getRawParameterValue("RATE")); // 0 = 1/2, 1 = 1/4, etc.

            double beatLength = 0.25; // fallback
            switch (rateIndex)
            {
                case 0: beatLength = 2.0; break;   // "1/2"
                case 1: beatLength = 1.0; break;   // "1/4"
                case 2: beatLength = 0.5; break;   // "1/8"
                case 3: beatLength = 0.25; break;  // "1/16"
                default: break;
            }
            
            double seqLengthInBeats = stepCount * beatLength;
            if (seqLengthInBeats > 0.0)
            {
                double beatInLoop = std::fmod(info.ppqPosition, seqLengthInBeats);
                currentStep = static_cast<int>(beatInLoop/beatLength);
                currentStepAtom.store(currentStep);
                
            }
        }
    }
    
    
    buffer.clear(); // Clear buffer before processing
    

    float stepValue = *apvts.getRawParameterValue("step" + juce::String(currentStep));  // Access the value of the current step
    

    synth.updateSynthParameters(apvts, stepValue);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    fxChainProcessor.process(buffer);

    
    
    
    
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
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN1", 1), "Pan 1", 0.0f, 1.0f, 0.5f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN2", 1), "Pan 2", 0.0f, 1.0f, 0.5f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN3", 1), "Pan 3", 0.0f, 1.0f, 0.5f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PAN4", 1), "Pan 4", 0.0f, 1.0f, 0.5f));
//
//    // Detune Parameters
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE1", 1), "Detune 1", -0.5f, 0.5f, 0.0f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE2", 1), "Detune 2", -0.5f, 0.5f, 0.0f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE3", 1), "Detune 3", -0.5f, 0.5f, 0.0f));
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE4", 1), "Detune 4", -0.5f, 0.5f, 0.0f));

    // Modulation Ratio Numerator and Denominator (1 to 16)
    for (int i = 1; i <= 4; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_RATIO_NUM" + std::to_string(i), 1),
            "Mod Ratio Numerator " + std::to_string(i),
            1, 5, 1));

        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_RATIO_DEN" + std::to_string(i), 1),
            "Mod Ratio Denominator " + std::to_string(i),
            1, 5, 1));
    }

    for (int i = 1; i <= 4; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("PAN" + std::to_string(i), 1), "Pan " + std::to_string(i), 0.0f, 1.0f, 0.5f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("DETUNE" + std::to_string(i), 1), "Detune " + std::to_string(i), -0.5f, 0.5f, 0.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("MOD_INDEX" + std::to_string(i), 1), "Mod Index " + std::to_string(i), 0.0f, 50.0f, 1.0f));
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("DOWNSAMPLE" + std::to_string(i), 1), "Downsample" + std::to_string(i), 0.0f, 50.0f, 1.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("MOD_WAVEFORM" + std::to_string(i), 1), "Mod Waveform " + std::to_string(i),
            juce::StringArray{ "Sine", "Saw", "Square" }, 0));

        parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("WAVEFORM" + std::to_string(i), 1), "Waveform " + std::to_string(i),
            juce::StringArray{ "Sine", "Saw", "Square" }, 0));
    }

    // Step Sequencer Parameters (8 steps)
    for (int i = 0; i < 8; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("step" + juce::String(i), 1),
            "Step " + juce::String(i + 1),
            0.0f, 1.0f, 1.0f));
    }
    parameters.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("STEP_COUNT", 1),
        "Step Count",
        1, 8, 8));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("RATE", 1),
        "RATE",
        juce::StringArray{ "1/2", "1/4", "1/8", "1/16" },
        2  // default = "1/8"
    ));
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("MOD_AMOUNT_ACTIVE", 1),
        "MOD_AMOUNT_ACTIVE",
        false));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("MOD_NUM_ACTIVE", 1),
        "MOD_NUM_ACTIVE",
        false));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("MOD_DEN_ACTIVE", 1),
        "MOD_DEN_ACTIVE",
        false));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahFreq", 1), "wahFreq", 1.0f, 50.0f, 10.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahFeedback", 1), "wahFeedback", 0.0f, 1.0f, 0.5f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahDelay", 1), "wahDelay", 5.0f, 50.0f, 20.0f));
    
    return { parameters.begin(), parameters.end() };
}
