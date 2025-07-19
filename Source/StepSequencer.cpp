#include "StepSequencer.h"

StepSequencer::StepSequencer(int numSteps_, int stepSeqIndex,
                             juce::AudioProcessorValueTreeState& vts)
    : numSteps(numSteps_), parameters(vts)
{
    
    // Set up the modulation list box with options.
//    modulationListBox.addItem("Pan", 1);
//    modulationListBox.addItem("Detune", 2);
//    modulationListBox.addItem("FM Amount", 3);
//    modulationListBox.addItem("Numerator", 4);
//    modulationListBox.addItem("Denominator", 5);
//    addAndMakeVisible(modulationListBox);
//    modChoices.
    seqIndex = stepSeqIndex;
    // Create the step sliders with attachments.
    for (int i = 0; i < numSteps; ++i)
    {
        auto slider = std::make_unique<juce::Slider>();
        slider->setSliderStyle(juce::Slider::LinearVertical);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
        auto paramID = "seq" + juce::String(stepSeqIndex) + "step"  + juce::String(i);
    
        auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, paramID, *slider);
    
        addAndMakeVisible(*slider);
        stepSliders.push_back(std::move(slider));
        sliderAttachments.push_back(std::move(attachment));
    }
    for (int i = 0; i < numSteps; ++i)
    {
        auto slider = std::make_unique<juce::Slider>();
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
        auto paramID = "seq" + juce::String(stepSeqIndex) + "offset"  + juce::String(i);
    
        auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, paramID, *slider);
    
        addAndMakeVisible(*slider);
        offsetKnobs.push_back(std::move(slider));
        offsetKnobAttachments.push_back(std::move(attachment));
    }
    
    rateSelector.addItem("4/1", 1);
    rateSelector.addItem("2/1", 2);
    rateSelector.addItem("1/1", 3);

    rateSelector.addItem("1/2", 4);
    rateSelector.addItem("1/2T", 5);
    rateSelector.addItem("1/2.", 6);

    rateSelector.addItem("1/4", 7);
    rateSelector.addItem("1/4T", 8);
    rateSelector.addItem("1/4.", 9);

    rateSelector.addItem("1/8", 10);
    rateSelector.addItem("1/8T", 11);
    rateSelector.addItem("1/8.", 12);

    rateSelector.addItem("1/16", 13);
    rateSelector.addItem("1/16T", 14);
    rateSelector.addItem("1/16.", 15);

    rateSelector.addItem("1/32", 16);
    rateSelector.addItem("1/32T", 17);
    rateSelector.addItem("1/32.", 18);


    addAndMakeVisible(rateSelector);
    
    modNumeratorButton.setButtonText("Numerator");
    modDenominatorButton.setButtonText("Denominator");
    modAmountButton.setButtonText("Mod Gain");
    modGainButton.setButtonText("Carrier Gain");
    
    addAndMakeVisible(modNumeratorButton);
    addAndMakeVisible(modDenominatorButton);
    addAndMakeVisible(modAmountButton);
    addAndMakeVisible(modGainButton);
    
    modAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters,  "seq" + juce::String(stepSeqIndex) + "MOD_AMOUNT_ACTIVE", modAmountButton);

    modNumeratorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters, "seq" + juce::String(stepSeqIndex) +"MOD_NUM_ACTIVE", modNumeratorButton);

    modDenominatorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters,  "seq" + juce::String(stepSeqIndex) +"MOD_DEN_ACTIVE", modDenominatorButton);
    
    modGainButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters,  "seq" + juce::String(stepSeqIndex) +"MOD_CARRIER_ACTIVE", modGainButton);
    
    
    stepCountSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    stepCountSlider.setRange(1, 8);
    stepCountSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(stepCountSlider);

    stepCountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters, "seq" + juce::String(stepSeqIndex) +"STEP_COUNT", stepCountSlider);
    rateSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        parameters, "seq" + juce::String(stepSeqIndex) +"RATE", rateSelector);
    
}

void StepSequencer::resized()
{
    stepCount = static_cast<int>(*parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "STEP_COUNT"));

    auto area = getLocalBounds();
    auto bottomArea = area.removeFromBottom(15);
    auto modulationArea = area.removeFromTop(40);
    auto offsetArea = area.removeFromTop(30);

    stepCountSlider.setBounds(bottomArea);

    int stepWidth = area.getWidth() / numSteps;

    // Layout and show active sliders
    for (int i = 0; i < numSteps; ++i)
    {
        if (i < stepCount)
        {
            stepSliders[i]->setBounds(i * stepWidth, area.getY(), stepWidth, area.getHeight());
            stepSliders[i]->setVisible(true);
        }
        else
        {
            stepSliders[i]->setVisible(false);
        }

        offsetKnobs[i]->setBounds(i * stepWidth, offsetArea.getY(), stepWidth, offsetArea.getHeight());
        // Optional: also hide offset knobs beyond stepCount
        offsetKnobs[i]->setVisible(i < stepCount);
    }

    rateSelector.setBounds(10, 10, 80, 24);
    modNumeratorButton.setBounds(150, 10, 120, 24);
    modDenominatorButton.setBounds(290, 10, 140, 24);
    modAmountButton.setBounds(440, 10, 120, 24);
    modGainButton.setBounds(590, 10, 120, 24);
}

void StepSequencer::paint(juce::Graphics& g)
{
    // Draw a dark blue background.
    g.fillAll(juce::Colour::fromFloatRGBA(0.125f, 0.25f, 0.5f, 1.0f));

    if (currentStep >= 0 && currentStep < stepSliders.size())
    {
        auto area = getLocalBounds();
        auto stepArea = area;
        stepArea.removeFromTop(30); // exclude modulation list box

        int stepWidth = stepArea.getWidth() / numSteps;

        // Highlight the current step with a fill
        g.setColour(juce::Colour::fromFloatRGBA(.125, 0.25f, 0.75f, 1));
        g.fillRect(currentStep * stepWidth, stepArea.getY(), stepWidth, stepArea.getHeight());

        // Draw border around active steps from 0 to currentStep (inclusive)
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.drawRect(0, stepArea.getY(), (stepCount ) * stepWidth, stepArea.getHeight()-15, 2); // thickness = 2
    }
}

void StepSequencer::updateFromHostPosition(double ppqPosition, double bpm)
{
    // Get the current rate setting
    int rateIndex = static_cast<int>(*parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "RATE"));
    
    // Convert rate index to beat length
    double beatLength = 0.25; // fallback
    switch (rateIndex)
    {
        case 0:  beatLength = 16.0; break;                  // "4/1"
        case 1:  beatLength = 8.0; break;                   // "2/1"
        case 2:  beatLength = 4.0; break;                   // "1/1"
        
        case 3:  beatLength = 2.0; break;                   // "1/2"
        case 4:  beatLength = 2.0 * 2.0 / 3.0; break;       // "1/2T"
        case 5:  beatLength = 2.0 * 1.5; break;             // "1/2."
        
        case 6:  beatLength = 1.0; break;                   // "1/4"
        case 7:  beatLength = 1.0 * 2.0 / 3.0; break;       // "1/4T"
        case 8:  beatLength = 1.0 * 1.5; break;             // "1/4."
        
        case 9:  beatLength = 0.5; break;                   // "1/8"
        case 10: beatLength = 0.5 * 2.0 / 3.0; break;       // "1/8T"
        case 11: beatLength = 0.5 * 1.5; break;             // "1/8."
        
        case 12: beatLength = 0.25; break;                 // "1/16"
        case 13: beatLength = 0.25 * 2.0 / 3.0; break;     // "1/16T"
        case 14: beatLength = 0.25 * 1.5; break;           // "1/16."
        
        case 15: beatLength = 0.125; break;                // "1/32"
        case 16: beatLength = 0.125 * 2.0 / 3.0; break;    // "1/32T"
        case 17: beatLength = 0.125 * 1.5; break;          // "1/32."
        

        
        default: break;
    }
    
    // Get the current step count
    stepCount = static_cast<int>(*parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "STEP_COUNT"));
    resized();
    // Calculate sequence length in beats
    double sequenceLengthInBeats = stepCount * beatLength;
    
    if (sequenceLengthInBeats > 0.0)
    {
        double beatInLoop = std::fmod(ppqPosition, sequenceLengthInBeats);
        if (beatInLoop < 0.0)
            beatInLoop += sequenceLengthInBeats; // always positive

        // Normalize beat position within the sequence
        double normalizedStepPos = beatInLoop / beatLength;
        int stepBase = static_cast<int>(normalizedStepPos) % stepCount;

        // Clamp in case of rounding issues
        
        
        
        int stepToTrigger = -1;

        for (int i = 0; i < stepCount; ++i)
        {
            auto* offsetParam = parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "offset" + juce::String(i));

            double stepStart = i + *offsetParam;
            double stepEnd   = i + 1.0 + *offsetParam; // you could also offset this if needed

            if (normalizedStepPos >= stepStart && normalizedStepPos < stepEnd)
            {
                stepToTrigger = i;
//                break;
            }
        }

        if (stepToTrigger >= 0)
        {
            currentStep = stepToTrigger;
            *parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "CURRENT_STEP") = currentStep;
            repaint();
        }
        
        
        
//        stepBase = juce::jlimit(0, stepCount - 1, stepBase);
//
//        // Get offset for the step (range -0.5 to 0.5)
//        auto* offsetParam = parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "offset" + juce::String(stepBase));
//        // Apply offset in step units, then recompute final step
//        double adjustedStepPos = normalizedStepPos + *offsetParam;
//        int adjustedStep = static_cast<int>(std::floor(adjustedStepPos)) % stepCount;
//        if (adjustedStep < 0)
//            adjustedStep += stepCount;

//        currentStep = adjustedStep;

        // Update the parameter for automation
        *parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "CURRENT_STEP") = currentStep;
        repaint();
        // Calculate current step
//        double beatInLoop = std::fmod(ppqPosition, sequenceLengthInBeats);
////        currentStep = static_cast<int>(beatInLoop / beatLength);
//        double stepFloat = beatInLoop / beatLength;
//        int stepBase = static_cast<int>(stepFloat);
//
//        // Clamp to range in case ppq is near edge
//        stepBase = juce::jlimit(0, stepCount - 1, stepBase);
//
//        // Fetch offset for this step
//        auto* offsetParam = parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "offset" + juce::String(stepBase));
//
//        // Apply offset as a fraction of beatLength
//        double adjustedStepFloat = (beatInLoop + *offsetParam * beatLength) / beatLength;
//
//        // Wrap around if needed
//        currentStep = static_cast<int>(std::floor(adjustedStepFloat)) % stepCount;
//        if (currentStep < 0)
//            currentStep += stepCount;
//        
//        // Update the parameter
//        *parameters.getRawParameterValue("seq" + juce::String(seqIndex) + "CURRENT_STEP") = currentStep;
//        
//        repaint();
    }
}

float StepSequencer::getCurrentAmplitude() const
{
    if (currentStep >= 0 && currentStep < stepSliders.size())
        return static_cast<float>(stepSliders[currentStep]->getValue());

    return 0.0f;
}
