#pragma once
#include <JuceHeader.h>

class FXUnit : public juce::Component
{
public:
    FXUnit() {}
    virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
};
