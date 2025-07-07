#include "FXChainProcessor.h"

FXChainProcessor::FXChainProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead) : apvts(apvtsRef), playHead(playHead)
{
    fxProcessors.push_back(std::make_unique<WahProcessor>(apvtsRef,playHead));
    fxProcessors.push_back(std::make_unique<ResonatorProcessor>(apvtsRef,playHead));
    fxProcessors.push_back(std::make_unique<Filter>(apvtsRef,playHead));
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
    for (auto& fx : fxProcessors){
        if (auto* res = dynamic_cast<ResonatorProcessor*>(fx.get()))
            res->setDelayLines (midiMessages);
        if (auto* filt = dynamic_cast<Filter*>(fx.get()))
            filt->update();
        fx->process(buffer);
        
    }
    
        
}
