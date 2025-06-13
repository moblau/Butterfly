#include "ResonatorComponent.h"

ResonatorComponent::ResonatorComponent(juce::AudioProcessorValueTreeState& apvtsRef)
    : FXUnit(apvtsRef)
{
    setSize(200, 200);

    // --- Feedback Slider ---
    feedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(feedbackSlider);

    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "res_feedback", feedbackSlider);

    // --- Dry/Wet Slider ---
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(dryWetSlider);

    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "res_drywet", dryWetSlider);

    // --- Ratio Slider ---
    ratioSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    ratioSlider.setRange(1.0, 4.0, 1.0);
    addAndMakeVisible(ratioSlider);
    
    filterFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    filterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    filterFreqSlider.setRange(20.0, 22000.0, 1.0);
    addAndMakeVisible(filterFreqSlider);
    filterFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "res_filterfreq", filterFreqSlider);
    
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "res_ratio", ratioSlider);
}

void ResonatorComponent::resized()
{
    const int padding = 10;
    const int knobSize = 80;
    const int sliderHeight = 40;

    feedbackSlider.setBounds(padding, padding, knobSize, knobSize);
    dryWetSlider.setBounds(getWidth() - knobSize - padding, padding, knobSize, knobSize);

    ratioSlider.setBounds(padding, knobSize + 2 * padding, getWidth()/2 - 2 * padding, sliderHeight);
    filterFreqSlider.setBounds(getWidth()/2, knobSize + 2 * padding, getWidth()/2 - 2 * padding, sliderHeight);
}
