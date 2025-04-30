#pragma once
#include "FXUnit.h"
#include "WahProcessor.h"

class AlienWahComponent : public FXUnit
{
public:
    AlienWahComponent();
    void resized() override;
    void paint(juce::Graphics& g) override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepare(double sampleRate, int samplesPerBlock);

private:
    WahProcessor processor;
};
