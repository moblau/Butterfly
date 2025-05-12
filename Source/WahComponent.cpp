#include "WahComponent.h"

WahComponent::WahComponent(juce::AudioProcessorValueTreeState& apvtsRef) : apvts(apvtsRef)
{
    setSize(200, 200);
    freqSlider.setSliderStyle(juce::Slider::Rotary);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    freqSlider.setRange(1.0, 50.0, 0.1);
    freqSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(freqSlider);
    freqAttachment.reset (new Attachment (apvtsRef, "wahFreq", freqSlider));

    // Feedback: 0–1
    feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    feedbackSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(feedbackSlider);
    feedbackAttachment.reset (new Attachment (apvtsRef, "wahFeedback", feedbackSlider));

    // Delay length: 5–50 samples
    delaySlider.setSliderStyle(juce::Slider::Rotary);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    delaySlider.setRange(5, 50, 1);
    delaySlider.setTextValueSuffix(" smp");
    addAndMakeVisible(delaySlider);
    delayAttachment.reset (new Attachment (apvtsRef, "wahDelay", delaySlider));
}

void WahComponent::resized() {
    freqSlider.setBounds(0, 20,66, 66);
    feedbackSlider.setBounds(66, 20,66, 66);
    delaySlider.setBounds(132, 20,66, 66);
}

void WahComponent::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
   g.setColour (juce::Colours::white);
   g.setFont (15.0f);
   g.drawFittedText ("Wah Controls", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
   
}
