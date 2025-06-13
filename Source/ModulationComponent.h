#pragma once

#include <JuceHeader.h>

// ------------------------------------
// EnvelopeComponent (standalone class)
// ------------------------------------
class EnvelopeComponent : public juce::Component
{
public:
    EnvelopeComponent(juce::AudioProcessorValueTreeState& apvtsRef, int i);
    void resized() override;

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, decayAttach, sustainAttach, releaseAttach;

    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

private:
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    juce::AudioProcessorValueTreeState& apvts;
};

// ------------------------------------
// ModulationComponent (main UI block)
// ------------------------------------
class ModulationComponent : public juce::Component
{
public:
    ModulationComponent(juce::AudioProcessorValueTreeState& apvtsRef);
    void resized() override;

private:
    EnvelopeComponent amplitudeEnvelope;
    EnvelopeComponent filterEnvelope;
    juce::TabbedComponent tabComponent { juce::TabbedButtonBar::TabsAtTop };
    juce::AudioProcessorValueTreeState& apvts;
};
