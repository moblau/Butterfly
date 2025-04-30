#include "WahComponent.h"

AlienWahComponent::AlienWahComponent()
{
    setSize(100, 120);
}

void AlienWahComponent::resized() {}

void AlienWahComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::purple);
    g.setColour(juce::Colours::white);
    g.drawText("Alien Wah", getLocalBounds(), juce::Justification::centred);
}

void AlienWahComponent::processBlock(juce::AudioBuffer<float>& buffer)
{
    processor.process(buffer);
}

void AlienWahComponent::prepare(double sampleRate, int samplesPerBlock)
{
    processor.prepare(sampleRate, samplesPerBlock, 1024);
}
