#pragma once
#include <JuceHeader.h>
#include "FXUnit.h"

class FXChain : public juce::Component
{
public:
    FXChain();
    void resized() override;
    void process(juce::AudioBuffer<float>& buffer);
    void prepare(double sampleRate, int samplesPerBlock);

private:
    std::vector<std::unique_ptr<FXUnit>> fxUnits;
};
