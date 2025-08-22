#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "ComponentUtility.h"
#include "ParamIDs.h"
class VoiceEditor : public juce::Component, private juce::Button::Listener
{
public:
    VoiceEditor(juce::AudioProcessorValueTreeState& apvtsRef, const juce::String& voicePrefix);
    ~VoiceEditor();

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

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
    
    juce::TextButton copyBtnA { "Copy" };
    juce::TextButton copyBtnB { "Copy" };
    juce::TextButton copyBtnC { "Copy" };

    // Targets for those three buttons (voice indices)
    int targetA = -1, targetB = -1, targetC = -1;
    
    void toggleAlias();
    bool shouldAlias = false;
    
    void copyVoiceParams(juce::AudioProcessorValueTreeState& apvts, int srcVoice, int dstVoice);
    
    juce::String voicePrefix;
    int voiceIndex;
    void buttonClicked(juce::Button* b) override;
    
  
    juce::Label copyLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceEditor)
};
