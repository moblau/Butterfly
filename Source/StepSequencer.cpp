#include "StepSequencer.h"

StepSequencer::StepSequencer(int numSteps_,
                             juce::AudioProcessorValueTreeState& vts,
                             const juce::String& prefix)
    : numSteps(numSteps_), parameters(vts), paramPrefix(prefix)
{
        for (int i = 0; i < numSteps; ++i)
        {
            auto slider = std::make_unique<juce::Slider>();
            slider->setSliderStyle(juce::Slider::LinearVertical);
            slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
            auto paramID = paramPrefix + juce::String(i);
    
            auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, paramID, *slider);
    
            addAndMakeVisible(*slider);
            stepSliders.push_back(std::move(slider));
            sliderAttachments.push_back(std::move(attachment));
        }
}

void StepSequencer::resized()
{
    auto area = getLocalBounds();
    int stepWidth = area.getWidth() / numSteps;

    for (int i = 0; i < numSteps; ++i)
    {
        stepSliders[i]->setBounds(i * stepWidth, 0, stepWidth, area.getHeight());
    }
}

void StepSequencer::paint(juce::Graphics& g)
{
    // Optional: Draw background or highlight current step
    g.fillAll(juce::Colours::darkgrey);

    if (currentStep >= 0 && currentStep < stepSliders.size())
    {
        auto area = getLocalBounds();
        int stepWidth = area.getWidth() / numSteps;
        g.setColour(juce::Colours::limegreen.withAlpha(0.3f));
        g.fillRect(currentStep * stepWidth, 0, stepWidth, area.getHeight());
    }
}

void StepSequencer::updateFromHostPosition(double ppqPosition, double bpm)
{
    double sequenceLengthInBeats = numSteps * stepLengthInBeats;

    if (sequenceLengthInBeats > 0.0)
    {
        double beatInLoop = std::fmod(ppqPosition, sequenceLengthInBeats);
        currentStep = static_cast<int>(beatInLoop / stepLengthInBeats);
        repaint(); // Visual feedback
    }
}

float StepSequencer::getCurrentAmplitude() const
{
    if (currentStep >= 0 && currentStep < stepSliders.size())
        return (float) stepSliders[currentStep]->getValue();

    return 0.0f;
}
