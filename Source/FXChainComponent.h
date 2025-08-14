#pragma once
#include <JuceHeader.h>
#include "FXUnit.h"
#include "WahComponent.h"
#include "ResonatorComponent.h"
#include "FilterComponent.h"

class FXChainComponent : public juce::Component
{
public:
    FXChainComponent(juce::AudioProcessorValueTreeState& state);
    void resized() override;
    void paint (juce::Graphics &g ) override;
private:
    std::vector<std::unique_ptr<FXUnit>> fxUIBlocks;
    juce::AudioProcessorValueTreeState& apvts;
};
