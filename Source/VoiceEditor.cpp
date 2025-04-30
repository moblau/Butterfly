#include "VoiceEditor.h"

VoiceEditor::VoiceEditor(juce::AudioProcessorValueTreeState& apvts, const juce::String& voicePrefix)
{
    setupSlider(panSlider);
    setupSlider(detuneSlider);
    setupSlider(modIndexSlider);
    setupSlider(modRatioNumSlider);
    setupSlider(modRatioDenSlider);
    setupSlider(downsampleSlider);
    setupComboBox(modWaveformBox);
    setupComboBox(waveformBox);
    
    panAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "PAN"+voicePrefix, panSlider);
    detuneAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "DETUNE"+voicePrefix, detuneSlider);
    modIndexAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_INDEX"+voicePrefix, modIndexSlider);
    modRatioNumAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_RATIO_NUM"+voicePrefix, modRatioNumSlider);
    modRatioDenAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_RATIO_DEN"+voicePrefix, modRatioDenSlider);
    downsampleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "DOWNSAMPLE"+voicePrefix, downsampleSlider);
    modWaveformAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,  "MOD_WAVEFORM"+voicePrefix, modWaveformBox);
    waveformAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,  "WAVEFORM"+voicePrefix, waveformBox);
}

void VoiceEditor::setupSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(slider);
}

void VoiceEditor::setupComboBox(juce::ComboBox& box)
{
    box.addItem("Sine", 1);
    box.addItem("Saw", 2);
    box.addItem("Square", 3);
    box.setSelectedId(1);
    addAndMakeVisible(box);
}

void VoiceEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    int padding = 10;
    int controlHeight = 60;

    waveformBox.setBounds(area.removeFromTop(24));
    area.removeFromTop(padding);
    
    panSlider.setBounds(area.removeFromTop(controlHeight));
    area.removeFromTop(padding);

    detuneSlider.setBounds(area.removeFromTop(controlHeight));
    area.removeFromTop(padding);

    modIndexSlider.setBounds(area.removeFromTop(controlHeight));
    area.removeFromTop(padding);

    modWaveformBox.setBounds(area.removeFromTop(24));
    area.removeFromTop(padding);

    modRatioNumSlider.setBounds(area.removeFromTop(controlHeight));
    area.removeFromTop(padding);

    modRatioDenSlider.setBounds(area.removeFromTop(controlHeight));
    downsampleSlider.setBounds(area.removeFromTop(controlHeight));
}

void updateSynthParameters(juce::AudioProcessorValueTreeState& apvts, float stepValue);
