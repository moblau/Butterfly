#include "ModulationComponent.h"

// =============================
// EnvelopeComponent definition
// =============================
EnvelopeComponent::EnvelopeComponent(juce::AudioProcessorValueTreeState& apvtsRef, int i) : apvts(apvtsRef)
{
    setupSlider(attackSlider, attackLabel, "Attack");
    setupSlider(decaySlider, decayLabel, "Decay");
    setupSlider(sustainSlider, sustainLabel, "Sustain");
    setupSlider(releaseSlider, releaseLabel, "Release");

    sustainSlider.setRange(0.0, 1, .01);
    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "attack" + juce::String(i), attackSlider);
    decayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "decay" + juce::String(i), decaySlider);

    sustainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "sustain" + juce::String(i), sustainSlider);

    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "release" + juce::String(i), releaseSlider);

    for (auto* comp : { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider,
                         })
        addAndMakeVisible(comp);
    
}

void EnvelopeComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setRange(0.0, 5000.0, 1.0);
    slider.setTextValueSuffix(" ms");
    slider.setSkewFactorFromMidPoint(500.0);
    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
}

void EnvelopeComponent::resized()
{
    auto area = getLocalBounds().reduced(25);
    auto sliderWidth = area.getWidth() / 4;

    attackSlider.setBounds(area.removeFromLeft(sliderWidth));
    decaySlider.setBounds(area.removeFromLeft(sliderWidth));
    sustainSlider.setBounds(area.removeFromLeft(sliderWidth));
    releaseSlider.setBounds(area);
}

// =============================
// ModulationComponent definition
// =============================
ModulationComponent::ModulationComponent(juce::AudioProcessorValueTreeState& apvtsRef) : apvts(apvtsRef), amplitudeEnvelope(apvtsRef,0), filterEnvelope(apvtsRef,1), modMatrix(apvtsRef)
{
    tabComponent.addTab("Amplitude", juce::Colours::lightblue, &amplitudeEnvelope, false);
    tabComponent.addTab("Filter", juce::Colours::lightgreen, &filterEnvelope, false);

    addAndMakeVisible(tabComponent);
    addAndMakeVisible(modMatrix);
}

void ModulationComponent::resized()
{
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop(area.getHeight()/2);
    tabComponent.setBounds(topArea.removeFromLeft(topArea.getWidth()/2));
    modMatrix.setBounds(topArea);
}
