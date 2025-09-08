#pragma once
#include "FXUnit.h"
#include "WahProcessor.h"

class WahComponent : public FXUnit
{
public:

    
    WahComponent(juce::AudioProcessorValueTreeState& state);
    void resized() override;
    void paint(juce::Graphics& g) override;
//    void processBlock(juce::AudioBuffer<float>& buffer) override;
//    void prepare(double sampleRate, int samplesPerBlock);

private:
//    juce::AudioProcessorValueTreeState& apvts;
//    juce::Slider freqSlider, feedbackSlider, delaySlider;
    
    SliderWithLabel freqSlider, feedbackSlider, delaySlider, offsetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment,
                                feedbackAttachment,
                                delayAttachment,
                                offsetAttachment;
    juce::ComboBox syncRateBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncRateAttachment;
    
    juce::ToggleButton syncToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncToggleAttachment;
    
    void updateSliderVisibility();

};
