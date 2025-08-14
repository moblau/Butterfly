#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "ComponentUtility.h"
class VoiceEditor : public juce::Component
{
public:
    VoiceEditor(juce::AudioProcessorValueTreeState& apvts, const juce::String& voicePrefix);
    ~VoiceEditor() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void placeLabelLeftOfSlider(juce::Label& label, const juce::Slider& slider, int labelWidth, int spacing);
    void placeLabelAboveSlider(juce::Label& label, const juce::Slider& slider, int labelHeight, int spacing );

    CustomLookAndFeel customLookAndFeel;
    
//    juce::Slider panSlider, detuneSlider, modIndexSlider, modRatioNumSlider, modRatioDenSlider, downsampleSlider;
    SliderWithLabel panSlider ;
    SliderWithLabel detuneSlider;
    SliderWithLabel modIndexSlider ;
    SliderWithLabel modRatioNumSlider;;
    SliderWithLabel modRatioDenSlider ;
    SliderWithLabel downsampleSlider ;
    SliderWithLabel freqSlider ;
    SliderWithLabel resSlider;
    
    SliderWithLabel resonatorOffsetSlider;
    SliderWithLabel resonatorFeedbackSlider;
    SliderWithLabel resonatorOffsetStrengthSlider;
    
//    juce::Label panSliderLabel, detuneSliderLabel, modIndexSliderLabel, modRatioNumSliderLabel, modRatioDenSliderLabel, downsampleSliderLabel;

    juce::ComboBox modWaveformBox, waveformBox;
    
    WaveformSelector carrierWaveform, modWaveform;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttach, detuneAttach, modIndexAttach, modRatioNumAttach, modRatioDenAttach, downsampleAttach, resOffsetAttach, resFeedbackAttach, resOffsetStrengthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modWaveformAttach, waveformAttach;

    std::unique_ptr<WaveformVisualizer> oscPreview;
//    juce::Slider freqSlider, resSlider;
//    juce::Label freqSliderLabel, resSliderLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttach, resAttach;
    
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void setupComboBox(juce::ComboBox& box);
    juce::ToggleButton aliasToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aliasToggleAttachment;
    
    void toggleAlias();
    bool shouldAlias = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceEditor)
};
