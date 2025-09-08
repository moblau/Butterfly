#include "FXChainProcessor.h"

FXChainProcessor::FXChainProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead) : apvts(apvtsRef), playHead(playHead)
{
    fxProcessors.push_back(std::make_unique<WahProcessor>(apvtsRef,playHead));
//    fxProcessors.push_back(std::make_unique<ResonatorProcessor>(apvtsRef,playHead));
//    fxProcessors.push_back(std::make_unique<Filter>(apvtsRef,playHead));
    //emplace back
//    fxProcessors.push_back(std::make_unique<ResonatorProcessor>(apvtsRef));
}

void FXChainProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    for (auto& fx : fxProcessors)
        fx->prepare(sampleRate, samplesPerBlock);
}

void FXChainProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    for (auto& fx : fxProcessors)
    {
        if (auto* wah = dynamic_cast<WahProcessor*>(fx.get()))
        {
            // Update parameters once per block
            wah->updateParameters();

            // Process each sample per channel
            for (int channel = 0; channel < numChannels; ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    channelData[sample] = wah->processSample(channelData[sample], channel);
                }
            }
        }
        else
        {
            // For other FX that still use buffer-level processing
            fx->process(buffer);
        }
    }
}
