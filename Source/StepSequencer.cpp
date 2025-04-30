#include "StepSequencer.h"

StepSequencer::StepSequencer(int numSteps_,
                             juce::AudioProcessorValueTreeState& vts,
                             const juce::String& prefix)
    : numSteps(numSteps_), parameters(vts), paramPrefix(prefix)
{
    // Set up the modulation list box with options.
//    modulationListBox.addItem("Pan", 1);
//    modulationListBox.addItem("Detune", 2);
//    modulationListBox.addItem("FM Amount", 3);
//    modulationListBox.addItem("Numerator", 4);
//    modulationListBox.addItem("Denominator", 5);
//    addAndMakeVisible(modulationListBox);
//    modChoices.
    
    // Create the step sliders with attachments.
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
    rateSelector.addItem("1/2", 1);
    rateSelector.addItem("1/4", 2);
    rateSelector.addItem("1/8", 3);
    rateSelector.addItem("1/16", 4);
    addAndMakeVisible(rateSelector);
    
    modNumeratorButton.setButtonText("Mod Numerator");
    modDenominatorButton.setButtonText("Mod Denominator");
    modAmountButton.setButtonText("Mod Amount");
    
    addAndMakeVisible(modNumeratorButton);
    addAndMakeVisible(modDenominatorButton);
    addAndMakeVisible(modAmountButton);
    
    modAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters,  "MOD_AMOUNT_ACTIVE", modAmountButton);

    modNumeratorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters, "MOD_NUM_ACTIVE", modNumeratorButton);

    modDenominatorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters,  "MOD_DEN_ACTIVE", modDenominatorButton);
    
    
    stepCountSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    stepCountSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(stepCountSlider);

    stepCountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters, "STEP_COUNT", stepCountSlider);
    rateSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        parameters, "RATE", rateSelector);
    
}

void StepSequencer::resized()
{
    auto area = getLocalBounds();
    auto bottomArea = area.removeFromBottom(15);
    stepCountSlider.setBounds(bottomArea);
    // Reserve the top 30 pixels for the modulation list box.
    auto modulationArea = area.removeFromTop(30);

    // The remaining area is used by the step sliders.
    int stepWidth = area.getWidth() / numSteps;
    for (int i = 0; i < numSteps; ++i)
    {
        stepSliders[i]->setBounds(i * stepWidth, area.getY(), stepWidth, area.getHeight());
    }
    rateSelector.setBounds(10, 10, 80, 24);
    modNumeratorButton.setBounds(100, 10, 120, 24);
    modDenominatorButton.setBounds(230, 10, 140, 24);
    modAmountButton.setBounds(380, 10, 120, 24);
}

void StepSequencer::paint(juce::Graphics& g)
{
    // Draw a dark grey background.
    g.fillAll(juce::Colours::blueviolet);
    // Highlight the current step.
    if (currentStep >= 0 && currentStep < stepSliders.size())
    {
        // Get the area excluding the modulation list box (top 30 pixels).
        auto area = getLocalBounds();
        auto stepArea = area;
        stepArea.removeFromTop(30);
        
        int stepWidth = stepArea.getWidth() / numSteps;
        g.setColour(juce::Colours::limegreen.withAlpha(0.3f));
        g.fillRect(currentStep * stepWidth, stepArea.getY(), stepWidth, stepArea.getHeight());
    }
}

void StepSequencer::updateFromHostPosition(double ppqPosition, double bpm)
{
    double sequenceLengthInBeats = numSteps * stepLengthInBeats;

    if (sequenceLengthInBeats > 0.0)
    {
        double beatInLoop = std::fmod(ppqPosition, sequenceLengthInBeats);
        currentStep = static_cast<int>(beatInLoop / stepLengthInBeats);
        repaint(); // Update visual feedback.
    }
}

float StepSequencer::getCurrentAmplitude() const
{
    if (currentStep >= 0 && currentStep < stepSliders.size())
        return static_cast<float>(stepSliders[currentStep]->getValue());

    return 0.0f;
}
