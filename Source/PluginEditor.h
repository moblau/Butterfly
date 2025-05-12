#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StepSequencer.h"
#include "VoiceEditor.h"
#include "FXChainComponent.h"

class ButterflyAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    ButterflyAudioProcessorEditor (ButterflyAudioProcessor&);
    ~ButterflyAudioProcessorEditor() override;
    
    void timerCallback() override;  // This will be called periodically to update the position

    void paint (juce::Graphics&) override;
    void resized() override;
//    int setCurrentStep() const { return stepSequencer.getCurrentStep();};
private:
    ButterflyAudioProcessor& audioProcessor;
    
    VoiceEditor voiceEditor1, voiceEditor2, voiceEditor3, voiceEditor4;


    StepSequencer stepSequencer;
    FXChainComponent fxChainComponent;
    
    void setupSlider(juce::Slider& slider, juce::Slider::SliderStyle style = juce::Slider::LinearHorizontal);
    void setupWaveformSelector(juce::ComboBox& box);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButterflyAudioProcessorEditor)
};
