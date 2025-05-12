#include "FXChainProcessor.h"

FXChainProcessor::FXChainProcessor(juce::AudioProcessorValueTreeState& apvtsRef) : apvts(apvtsRef)
{
    fxProcessors.push_back(std::make_unique<WahProcessor>(apvtsRef));
}

void FXChainProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    for (auto& fx : fxProcessors)
        fx->prepare(sampleRate, samplesPerBlock);
}

void FXChainProcessor::process(juce::AudioBuffer<float>& buffer)
{
    for (auto& fx : fxProcessors)
        fx->process(buffer);
}
