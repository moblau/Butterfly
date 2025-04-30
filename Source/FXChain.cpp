#include "FXChain.h"
#include "WahComponent.h"

FXChain::FXChain()
{
    auto fx = std::make_unique<WahComponent>();
    addAndMakeVisible(*fx);
    fxUnits.push_back(std::move(fx));
}

void FXChain::resized()
{
    auto area = getLocalBounds();
    for (auto& fx : fxUnits)
        fx->setBounds(area.removeFromTop(120).reduced(5));
}

void FXChain::process(juce::AudioBuffer<float>& buffer)
{
    for (auto& fx : fxUnits)
        fx->processBlock(buffer);
}

void FXChain::prepare(double sampleRate, int samplesPerBlock)
{
    for (auto& fx : fxUnits)
        if (auto* wah = dynamic_cast<AlienWahComponent*>(fx.get()))
            wah->prepare(sampleRate, samplesPerBlock);
}
