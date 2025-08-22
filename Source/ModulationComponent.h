#pragma once

#include <JuceHeader.h>
#include "StepSequencer.h"

// ------------------------------------
// EnvelopeComponent (standalone class)
// ------------------------------------
class EnvelopeComponent : public juce::Component
{
public:
    EnvelopeComponent(juce::AudioProcessorValueTreeState& apvtsRef, int i);
    void resized() override;
    void paint(juce::Graphics& g) override;

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, decayAttach, sustainAttach, releaseAttach;

    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

private:
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    juce::AudioProcessorValueTreeState& apvts;
};

class ModMatrixComponent : public juce::Component
{
public:
    ModMatrixComponent(juce::AudioProcessorValueTreeState& state)
    : apvts(state)
    {
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

        for (int mod = 1; mod <= 4; ++mod)
        {
            for (int car = 1; car <= 4; ++car)
            {
                auto slider = std::make_unique<juce::Slider>();
                slider->setSliderStyle(juce::Slider::LinearVertical);
                slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
                slider->setTextValueSuffix(""); // No suffix
                slider->setRange(0.0f, 100.0f, 1.0f);

                auto paramID = "mod_" + std::to_string(mod) + "_" + std::to_string(car);
                attachments.push_back(std::make_unique<SliderAttachment>(apvts, paramID, *slider));

                sliders.push_back(std::move(slider));
                addAndMakeVisible(sliders.back().get());
            }
        }
        addAndMakeVisible(modMatrixTitle);
        modMatrixTitle.setText("Intermodulation Matrix", juce::NotificationType::dontSendNotification);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto cellW = bounds.getWidth() / 4;
        auto cellH = bounds.getHeight() / 4;

        int idx = 0;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                sliders[idx++]->setBounds(col * cellW, row * cellH, cellW, cellH);
            }
        }
    }
    void paint(juce::Graphics& g) override
    {
        auto border = getLocalBounds();
        auto title = border.removeFromTop(border.getHeight()*.08);
        modMatrixTitle.setBounds(title);
        g.setColour(juce::Colours::grey);
        g.drawRect(border,1);
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    juce::Label modMatrixTitle;
};

// ------------------------------------
// ModulationComponent (main UI block)
// ------------------------------------
class ModulationComponent : public juce::Component
{
public:
    ModulationComponent(juce::AudioProcessorValueTreeState& apvtsRef);
    void resized() override;
    StepSequencer modSequencer;
    private:
    EnvelopeComponent amplitudeEnvelope;
    EnvelopeComponent filterEnvelope;
    juce::TabbedComponent tabComponent { juce::TabbedButtonBar::TabsAtTop };
    juce::AudioProcessorValueTreeState& apvts;
    
    
    ModMatrixComponent modMatrix;
};

