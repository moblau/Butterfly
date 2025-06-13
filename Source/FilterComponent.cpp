#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvtsRef)
    : FXUnit(apvtsRef)
{
    setSize(200, 200);

    // --- Feedback Slider ---
    freqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    freqSlider.setRange(20, 22000.0, 1);
    addAndMakeVisible(freqSlider);

    freqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_freq", freqSlider);

    // --- Dry/Wet Slider ---
    resSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    resSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    resSlider.setRange(.5, 3.0, .1);
    addAndMakeVisible(resSlider);

    resSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_res", resSlider);

    // --- Ratio Slider ---
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    driveSlider.setRange(1.0, 4.0, 1.0);
    addAndMakeVisible(driveSlider);
    
    driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_drive", driveSlider);
    
}

void FilterComponent::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Top row for freq and res
    auto topRow = area.removeFromTop(100);
    auto freqArea = topRow.removeFromLeft(topRow.getWidth() / 2);
    freqSlider.setBounds(freqArea);
    resSlider.setBounds(topRow); // remaining right half

    // Bottom left for drive slider
//    auto bottomRow = area.removeFromTop(100); // leave room at bottom
    auto driveWidth = getWidth() / 2;
    driveSlider.setBounds(area.removeFromLeft(driveWidth));
}
