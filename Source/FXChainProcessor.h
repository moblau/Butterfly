#pragma once
#include <JuceHeader.h>
#include "WahProcessor.h"
#include "DSPModule.h"
#include "ResonatorProcessor.h"
#include "Filter.h"
class FXChainProcessor
{
public:
    FXChainProcessor(juce::AudioProcessorValueTreeState& apvtsRef , juce::AudioPlayHead* playHead);
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

private:
    std::vector<std::unique_ptr<DSPModule>> fxProcessors;
    //no need for unique ptr, allocated on stack, use std::array, use array unique ptr to allocate on heap
    juce::AudioProcessorValueTreeState& apvts;
    juce::AudioPlayHead* playHead = nullptr;
};
