#pragma once
#include <JuceHeader.h>
#include "FXUnit.h"

class FilterComponent : public FXUnit
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& apvtsRef);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:

    SliderWithLabel freqSlider, resSlider, envSlider, driveSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqSliderAttachment, driveSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resSliderAttachment, envSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
