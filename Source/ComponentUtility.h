/*
  ==============================================================================

    ComponentUtility.h
    Created: 18 Jul 2025 5:43:46am
    Author:  Morris Blaustein

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

class SliderWithLabel : public juce::Component
{
public:
    SliderWithLabel() = default;
    
    SliderWithLabel(juce::String newLabel){
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(slider);

        label.setText(newLabel, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }
    ~SliderWithLabel() override
    {
        slider.setLookAndFeel(nullptr); // IMPORTANT: prevent dangling pointer
    }
    juce::Slider& getSlider() { return slider; }
    juce::Label& getLabel() { return label; }
    
    void resized()
    {
        auto bounds = getLocalBounds();
        label.setBounds(bounds.removeFromTop(20));
        slider.setBounds(bounds);
    }
private:
    juce::Slider slider;
    juce::Label label;
    CustomLookAndFeel customLookAndFeel;
};

class WaveformSelector : public juce::Component, private juce::AudioProcessorValueTreeState::Listener
{
public:
    
    WaveformSelector(juce::AudioProcessorValueTreeState& apvts, juce::String waveformType, juce::String waveformIndex)
    {
        // Sine button
        sineOn = juce::ImageFileFormat::loadFrom(BinaryData::SineOn_png, BinaryData::SineOn_pngSize);
        sineOff = juce::ImageFileFormat::loadFrom(BinaryData::SineOff_png, BinaryData::SineOff_pngSize);
        sineButton.setImages(false, true, true, sineOff, 1.0f, {}, sineOn, 1.0f, {}, sineOn, 1.0f, {});
        addAndMakeVisible(sineButton);
        sineButton.onClick = [this]() { selectButton(&sineButton); };

        // Saw button
        sawOn = juce::ImageFileFormat::loadFrom(BinaryData::SawOn_png, BinaryData::SawOn_pngSize);
        sawOff = juce::ImageFileFormat::loadFrom(BinaryData::SawOff_png, BinaryData::SawOff_pngSize);
        sawButton.setImages(false, true, true, sawOff, 1.0f, {}, sawOn, 1.0f, {}, sawOn, 1.0f, {});
        addAndMakeVisible(sawButton);
        sawButton.onClick = [this]() { selectButton(&sawButton); };

        // Square button
        squareOn = juce::ImageFileFormat::loadFrom(BinaryData::SquareOn_png, BinaryData::SquareOn_pngSize);
        squareOff = juce::ImageFileFormat::loadFrom(BinaryData::SquareOff_png, BinaryData::SquareOff_pngSize);
        squareButton.setImages(false, true, true, squareOff, 1.0f, {}, squareOn, 1.0f, {}, squareOn, 1.0f, {});
        addAndMakeVisible(squareButton);
        squareButton.onClick = [this]() { selectButton(&squareButton); };
        
        auto paramID = waveformType + waveformIndex;

        bindToParameter(apvts, paramID);

        selectButton(&sineButton);

    }

    void bindToParameter(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
    {
        parameter = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramID));
        jassert(parameter != nullptr);
        apvts.addParameterListener(paramID, this);

        setWaveformFromIndex(parameter->getIndex());
    }


    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        auto buttonWidth = area.getWidth() / 3;

        sineButton.setBounds(area.removeFromLeft(buttonWidth));
        sawButton.setBounds(area.removeFromLeft(buttonWidth));
        squareButton.setBounds(area);
    }


private:
    juce::ImageButton sineButton, sawButton, squareButton;
    juce::Image sineOn, sineOff, sawOn, sawOff, squareOn, squareOff;
    juce::String currentSelection;

    juce::AudioParameterChoice* parameter = nullptr;

    void selectButton(juce::ImageButton* selected)
    {
        auto set = [](juce::ImageButton& b, const juce::Image& on, const juce::Image& off, bool isOn)
        {
            b.setImages(false, true, true,
                        isOn ? on : off, 1.0f, {},
                        isOn ? on : off, 1.0f, {},
                        isOn ? on : off, 1.0f, {});
        };

        set(sineButton, sineOn, sineOff, selected == &sineButton);
        set(sawButton, sawOn, sawOff, selected == &sawButton);
        set(squareButton, squareOn, squareOff, selected == &squareButton);

        if (parameter != nullptr)
        {
            parameter->beginChangeGesture();
            if (selected == &sineButton)   parameter->setValueNotifyingHost(0.0f); // index 0
            if (selected == &sawButton)    parameter->setValueNotifyingHost(.5f);
            if (selected == &squareButton) parameter->setValueNotifyingHost(1.0f);
            parameter->endChangeGesture();
        }
    }

    void setWaveformFromIndex(int index)
    {
        if      (index == 0) selectButton(&sineButton);
        else if (index == 1) selectButton(&sawButton);
        else if (index == 2) selectButton(&squareButton);
    }
//
    void parameterChanged(const juce::String&, float newValue) override
    {
        juce::MessageManager::callAsync([this, index = static_cast<int>(newValue)] {
            setWaveformFromIndex(index);
        });
    }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformSelector)
};

