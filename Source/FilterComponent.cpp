#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvtsRef)
: FXUnit(apvtsRef), freqSlider("Frequency", apvtsRef, juce::String(6)),
resSlider("Resonance", apvtsRef, juce::String(6)),
envSlider("Envelope", apvtsRef, juce::String(6)),
driveSlider("Drive", apvtsRef, juce::String(6))
{
    setSize(200, 200);

    // --- Feedback Slider ---

    addAndMakeVisible(freqSlider);

    freqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_freq", freqSlider.getSlider());

    // --- Dry/Wet Slider ---

    addAndMakeVisible(resSlider);

    resSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_res", resSlider.getSlider());

    addAndMakeVisible(envSlider);
    
    envSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_env", envSlider.getSlider());
    
    
    
    addAndMakeVisible(driveSlider);
    
    driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, "filter_drive", driveSlider.getSlider());
    

}

void FilterComponent::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(30);
    // Top row for freq and res
    auto topRow = area.removeFromTop(area.getHeight()/2);
    auto freqArea = topRow.removeFromLeft(topRow.getWidth() / 2);
    freqSlider.setBounds(freqArea);
    resSlider.setBounds(topRow); // remaining right half
    envSlider.setBounds(area.removeFromLeft(area.getWidth()/2));
    driveSlider.setBounds(area);
    // Bottom left for drive slider
//    auto bottomRow = area.removeFromTop(100); // leave room at bottom
//    auto driveWidth = getWidth() / 2;
//    driveSlider.setBounds(area.removeFromLeft(driveWidth));
}

void FilterComponent::paint(juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Global Filter", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
    
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds,1);
}
