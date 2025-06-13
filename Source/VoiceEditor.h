#pragma once

#include <JuceHeader.h>

class VoiceEditor : public juce::Component
{
public:
    VoiceEditor(juce::AudioProcessorValueTreeState& apvts, const juce::String& voicePrefix);
    ~VoiceEditor() override = default;

    void resized() override;

private:
    juce::Slider panSlider, detuneSlider, modIndexSlider, modRatioNumSlider, modRatioDenSlider, downsampleSlider;
    juce::ComboBox modWaveformBox, waveformBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttach, detuneAttach, modIndexAttach, modRatioNumAttach, modRatioDenAttach, downsampleAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modWaveformAttach, waveformAttach;

    void setupSlider(juce::Slider& slider);
    void setupComboBox(juce::ComboBox& box);
    juce::ToggleButton aliasToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aliasToggleAttachment;
    
    void toggleAlias();
    bool shouldAlias = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceEditor)
};
