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
apvts(*this, nullptr, "PARAMETERS", createParameters()), fxChainProcessor(apvts, getPlayHead()), synth(apvts)
{

    // ... repeat for 4 voices
    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
    {
        auto* voice = new FMVoice();
        synth.addVoice (voice);

        // Now that we're in prepareToPlay, we know sampleRate & blockSize:
    }
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
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<FMVoice*>(synth.getVoice(i))){
            voice->setVoiceId(i);
            voice->prepare(sampleRate, samplesPerBlock);
        }
    }
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
//    if (auto* playhead = getPlayHead()){
//        juce::AudioPlayHead::CurrentPositionInfo info;
//        if ( playhead->getCurrentPosition(info)){
//            int stepCount = static_cast<int>(*apvts.getRawParameterValue("seq1STEP_COUNT"));
//            
//            int rateIndex = static_cast<int>(*apvts.getRawParameterValue("seq1RATE")); // 0 = 1/2, 1 = 1/4, etc.
//
//            double beatLength = 0.25; // fallback
//            switch (rateIndex)
//            {
//                case 0: beatLength = 2.0; break;   // "1/2"
//                case 1: beatLength = 1.0; break;   // "1/4"
//                case 2: beatLength = 0.5; break;   // "1/8"
//                case 3: beatLength = 0.25; break;  // "1/16"
//                default: break;
//            }
//            
//            double seqLengthInBeats = stepCount * beatLength;
//            if (seqLengthInBeats > 0.0)
//            {
//                double beatInLoop = std::fmod(info.ppqPosition, seqLengthInBeats);
//                currentStep = static_cast<int>(beatInLoop/beatLength);
//            }
//        }
//    }
    
    buffer.clear(); // Clear buffer before processing
    
//    int currentStep1 = static_cast<int>(*apvts.getRawParameterValue("seq1CURRENT_STEP"));
//    
//    
//    float stepValue1 = *apvts.getRawParameterValue("seq1step" + juce::String(currentStep1));  // Access the value of the current step
    
    juce::MidiBuffer midiCopy(midiMessages);
    synth.updateSynthParameters();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
//    fxChainProcessor.process(buffer, midiCopy );

    
    
    
    
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
    
    
    //use emplace back vs push back
    //use reserve to allocate how many parameters.
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
    for (int j = 1; j <= 4; ++j)
    {
        for (int i = 0; i < 8; ++i)
        {
            parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID("seq" + juce::String(j) + "step" + juce::String(i), 1),
                "seq" + juce::String(j) + "step" + juce::String(i),
                0.0f, 1.0f, 1.0f));
            parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID("seq" + juce::String(j) + "offset" + juce::String(i), 1),
                "seq" + juce::String(j) + "offset" + juce::String(i),
                -.5f, 0.5f, .01f));
        }
    }

    
    for (int j = 1; j <= 4; ++j)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("seq" + juce::String(j) + "STEP_COUNT", 1),
            "seq" + juce::String(j) + "STEP_COUNT",
            1, 8, 8));

        parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("seq" + juce::String(j) + "RATE", 1),
            "seq" + juce::String(j) + "RATE",
            juce::StringArray{
                "4/1", "2/1", "1/1",
                "1/2", "1/2T", "1/2.",
                "1/4", "1/4T", "1/4.",
                "1/8", "1/8T", "1/8.",
                "1/16", "1/16T", "1/16.",
                "1/32", "1/32T", "1/32."
            },
            2 // default index: "1/1"
        ));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID("seq" + juce::String(j) + "MOD_AMOUNT_ACTIVE", 1),
            "seq" + juce::String(j) + "MOD_AMOUNT_ACTIVE",
            false));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID("seq" + juce::String(j) + "MOD_NUM_ACTIVE", 1),
            "seq" + juce::String(j) + "MOD_NUM_ACTIVE",
            false));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID("seq" + juce::String(j) + "MOD_DEN_ACTIVE", 1),
            "seq" + juce::String(j) + "MOD_DEN_ACTIVE",
            false));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID("seq" + juce::String(j) + "MOD_CARRIER_ACTIVE", 1),
            "seq" + juce::String(j) + "MOD_CARRIER_ACTIVE",
            false));
        
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("seq" + juce::String(j) + "CURRENT_STEP", 1),
           "seq" + juce::String(j) + "CURRENT_STEP",
            0, 7, 0));
    }
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahFreq", 1), "wahFreq", 1.0f, 50.0f, 10.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahFeedback", 1), "wahFeedback", 0.0f, 1.0f, 0.5f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wahDelay", 1), "wahDelay", 5.0f, 50.0f, 20.0f));
    
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("WahUseSync", 1),                 // ID
      "WahUseSync",                     // UI name
        false));                        // default = false (use free freq)

    // 2) Choice for sync rate (quarter, eighth, sixteenth, etc.)
    juce::StringArray syncChoices { "1/4", "1/8", "1/16", "1/32" };
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID("WahSyncRate", 1),                // ID
        "WahSyncRate",                    // UI name
        syncChoices,
        1));
    
    for (int i = 1; i <= 4; ++i)
    {
        auto paramName = std::to_string(i) + "AliasToggle";

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(paramName, 1),  // e.g. "1AliasToggle", "2AliasToggle", …
            paramName,                       // e.g. "1 AliasToggle", "2 AliasToggle", …
            false
        ));
    }
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("res_feedback", 1), "res_feedback", 0.0f, 1.0f, 0.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("res_drywet", 1), "res_drywet", 0.0f, 1.0f, 0.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("res_ratio", 1), "res_drywet", 1.0f, 4.0f, 1.0f));
    
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("res_filterfreq", 1), "res_filterfreq", 20.0f,22000.0f, 1000.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter_freq", 1),
        "filter_freq",
        juce::NormalisableRange<float>(20.0f, 22000.0f, 0.01f, 0.25f), // Skewed for perceptual tuning
        1000.0f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter_res", 1),
        "filter_res",
        juce::NormalisableRange<float>(0.1f, 3.0, 0.01f),
        1.0f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter_drive", 1),
        "filter_drive",
        juce::NormalisableRange<float>(1.0f, 4.0f, 1.0f),
        1.0f));
    juce::NormalisableRange<float> envRange { 0.0f, 5000.0f, 1.0f };
    envRange.setSkewForCentre(500.0f); // midpoint skew

    for (int i = 0; i < 2; ++i)
    {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("attack" + juce::String(i), 1),
            "attack" + juce::String(i),
            envRange,
            10.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("decay" + juce::String(i), 1),
            "decay" + juce::String(i),
            envRange,
            100.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("sustain" + juce::String(i), 1),
            "sustain" + juce::String(i),
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            1.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("release" + juce::String(i), 1),
            "release" + juce::String(i),
            envRange,
            200.0f));
    }

    return { parameters.begin(), parameters.end() };
}
