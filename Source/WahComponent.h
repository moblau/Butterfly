#pragma once
#include "FXUnit.h"
#include "WahProcessor.h"

class WahComponent : public FXUnit
{
public:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    WahComponent(juce::AudioProcessorValueTreeState& state);
    void resized() override;
    void paint(juce::Graphics& g) override;
//    void processBlock(juce::AudioBuffer<float>& buffer) override;
//    void prepare(double sampleRate, int samplesPerBlock);

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::Slider freqSlider, feedbackSlider, delaySlider;
    std::unique_ptr<Attachment> freqAttachment,
                                feedbackAttachment,
                                delayAttachment;
};
