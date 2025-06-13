/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FMVoice.h"
#include "FMSynth.h"
#include "FXChainProcessor.h"

//==============================================================================
/**
*/
class ButterflyAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ButterflyAudioProcessor();
    ~ButterflyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioParameterFloat* panParam1;
    juce::AudioParameterFloat* panParam2;
    juce::AudioParameterFloat* panParam3;
    juce::AudioParameterFloat* panParam4;
    juce::AudioParameterFloat* modRatioParam1;
    juce::AudioParameterFloat* modRatioParam2;
    juce::AudioParameterFloat* modRatioParam3;
    juce::AudioParameterFloat* modRatioParam4;
    juce::AudioProcessorValueTreeState apvts;
    
private:
    FMSynth synth;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    FXChainProcessor fxChainProcessor;
    
    juce::dsp::Gain<float> gainProcessor;
//    WahProc alienWah;

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButterflyAudioProcessor)
};
