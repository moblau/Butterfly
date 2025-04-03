#pragma once

#include <JuceHeader.h>

class StepSequencer : public juce::Component
{
public:
    StepSequencer(int numSteps,
                  juce::AudioProcessorValueTreeState& vts,
                  const juce::String& paramPrefix);

    void resized() override;
    void paint(juce::Graphics& g) override;

    void updateFromHostPosition(double ppqPosition, double bpm);
    float getCurrentAmplitude() const;
    int getCurrentStep() const { return currentStep; }
private:
    int numSteps;
    int currentStep = 0;

    juce::AudioProcessorValueTreeState& parameters;
    juce::String paramPrefix;

    double stepLengthInBeats = 0.25; // 1/16th note by default

    std::vector<std::unique_ptr<juce::Slider>> stepSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepSequencer)
};
