#pragma once
#include <JuceHeader.h>
#include "WahProcessor.h"
#include "DSPModule.h"
class FXChainProcessor
{
public:
    FXChainProcessor(juce::AudioProcessorValueTreeState& apvtsRef );
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);

private:
    std::vector<std::unique_ptr<DSPModule>> fxProcessors;
    juce::AudioProcessorValueTreeState& apvts;
};
