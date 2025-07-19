#pragma once

#include <JuceHeader.h>

class StepSequencer : public juce::Component
{
public:
    StepSequencer(int numSteps, int stepSeqIndex,
                  juce::AudioProcessorValueTreeState& vts);

    void resized() override;
    void paint(juce::Graphics& g) override;

    void updateFromHostPosition(double ppqPosition, double bpm);
    float getCurrentAmplitude() const;
    int setCurrentStep(int step) { currentStep = step; }

private:
    int numSteps;
    int currentStep = 0;
    int stepCount;
    int seqIndex;
    juce::AudioProcessorValueTreeState& parameters;
    juce::String paramPrefix;

    double stepLengthInBeats = 0.25; // 1/16th note by default

    std::vector<std::unique_ptr<juce::Slider>> stepSliders;
    std::vector<std::unique_ptr<juce::Slider>> offsetKnobs;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> offsetKnobAttachments;

    // New modulation selector (list box)
    juce::ComboBox rateSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rateSelectorAttachment;
    
    juce::ToggleButton modNumeratorButton;
    juce::ToggleButton modDenominatorButton;
    juce::ToggleButton modAmountButton;
    juce::ToggleButton modGainButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modNumeratorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modDenominatorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modGainButtonAttachment;
    
    
    juce::Slider stepCountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepCountSliderAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepSequencer)
};
