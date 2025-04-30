#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StepSequencer.h"
#include "VoiceEditor.h"

class ButterflyAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    ButterflyAudioProcessorEditor (ButterflyAudioProcessor&);
    ~ButterflyAudioProcessorEditor() override;
    
    void timerCallback() override;  // This will be called periodically to update the position

    void paint (juce::Graphics&) override;
    void resized() override;
//    int setCurrentStep() const { return stepSequencer.getCurrentStep();};
private:
    ButterflyAudioProcessor& audioProcessor;
    
    VoiceEditor voiceEditor1, voiceEditor2, voiceEditor3, voiceEditor4;
//    // Pan Sliders
//    juce::Slider panSlider1, panSlider2, panSlider3, panSlider4;
//
//    // Modulation Index Sliders (Radial)
//    juce::Slider modIndexSlider1, modIndexSlider2, modIndexSlider3, modIndexSlider4;
//
//    // Detune Sliders
//    juce::Slider detuneSlider1, detuneSlider2, detuneSlider3, detuneSlider4;
//
//    // Modulator Waveform Selectors
//    juce::ComboBox modWaveformBox1, modWaveformBox2, modWaveformBox3, modWaveformBox4;
//
//    // Modulation Ratio Sliders (Numerator / Denominator)
//    juce::Slider modRatioNumSlider1, modRatioDenSlider1;
//    juce::Slider modRatioNumSlider2, modRatioDenSlider2;
//    juce::Slider modRatioNumSlider3, modRatioDenSlider3;
//    juce::Slider modRatioNumSlider4, modRatioDenSlider4;
//
//    // Attachments
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttach1, panAttach2, panAttach3, panAttach4;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modIndexAttach1, modIndexAttach2, modIndexAttach3, modIndexAttach4;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttach1, detuneAttach2, detuneAttach3, detuneAttach4;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modWaveformAttach1, modWaveformAttach2, modWaveformAttach3, modWaveformAttach4;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRatioNumAttach1, modRatioDenAttach1;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRatioNumAttach2, modRatioDenAttach2;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRatioNumAttach3, modRatioDenAttach3;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRatioNumAttach4, modRatioDenAttach4;

    StepSequencer stepSequencer;

    void setupSlider(juce::Slider& slider, juce::Slider::SliderStyle style = juce::Slider::LinearHorizontal);
    void setupWaveformSelector(juce::ComboBox& box);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButterflyAudioProcessorEditor)
};
