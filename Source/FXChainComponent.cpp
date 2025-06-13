#include "FXChainComponent.h"

FXChainComponent::FXChainComponent(juce::AudioProcessorValueTreeState& apvtsRef) : apvts(apvtsRef)
{
    auto wahUI = std::make_unique<WahComponent>(apvtsRef);
    auto resUI = std::make_unique<ResonatorComponent>(apvtsRef);
    auto filterUI = std::make_unique<FilterComponent>(apvtsRef);
    addAndMakeVisible(*wahUI);
    addAndMakeVisible(*resUI);
    addAndMakeVisible(*filterUI);
    fxUIBlocks.push_back(std::move(wahUI));
    fxUIBlocks.push_back(std::move(resUI));
    fxUIBlocks.push_back(std::move(filterUI));
}

void FXChainComponent::resized()
{
    auto area = getLocalBounds();
    for (auto& fx : fxUIBlocks)
        fx->setBounds(area.removeFromTop(200).reduced(5));
}
