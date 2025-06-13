#pragma once
#include <JuceHeader.h>

class FXUnit : public juce::Component
{
public:
    FXUnit(juce::AudioProcessorValueTreeState& apvtsRef);
    
private:
    juce::AudioProcessorValueTreeState& apvts;

};
