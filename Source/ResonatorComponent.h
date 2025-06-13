#pragma once
#include <JuceHeader.h>
#include "FXUnit.h"

class ResonatorComponent : public FXUnit
{
public:
    ResonatorComponent(juce::AudioProcessorValueTreeState& apvtsRef);
    void resized() override;

private:
    juce::Slider feedbackSlider;
    juce::Slider dryWetSlider;
    juce::Slider ratioSlider;
    juce::Slider filterFreqSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterFreqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorComponent)
};
