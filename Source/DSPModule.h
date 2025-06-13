#pragma once

#include <JuceHeader.h>

class DSPModule {
public:
    DSPModule(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead) : apvts(apvtsRef), playHead(playHead) {}
    virtual ~DSPModule() = default;

    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    virtual void reset() = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;

protected:
    juce::AudioProcessorValueTreeState& apvts;
    juce::AudioPlayHead* playHead = nullptr;
};

//class DSPModule
//{
//public:
//    DSPModule( juce::AudioProcessorValueTreeState& apvtsRef ) : apvts(apvtsRef){
//        
//    }
//    virtual ~DSPModule() = default;
//
//    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
//    virtual void reset() = 0;
//
//    // This is the key:
//    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
//    
//    juce::AudioProcessorValueTreeState& apvts;
//};
