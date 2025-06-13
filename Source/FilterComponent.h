#pragma once
#include <JuceHeader.h>
#include "FXUnit.h"

class FilterComponent : public FXUnit
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& apvtsRef);
    void resized() override;

private:
    juce::Slider freqSlider;
    juce::Slider resSlider;
    juce::Slider driveSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
