#include "FXChainComponent.h"

FXChainComponent::FXChainComponent(juce::AudioProcessorValueTreeState& apvtsRef) : apvts(apvtsRef)
{
    auto wahUI = std::make_unique<WahComponent>(apvtsRef);
    addAndMakeVisible(*wahUI);
    fxUIBlocks.push_back(std::move(wahUI));
}

void FXChainComponent::resized()
{
    auto area = getLocalBounds();
    for (auto& fx : fxUIBlocks)
        fx->setBounds(area.removeFromTop(200).reduced(5));
}
