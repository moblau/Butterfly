#include <JuceHeader.h>
#include "VoiceEditor.h"
#include "StepSequencer.h"

class VoiceTabComponent : public juce::Component
{
public:
    VoiceTabComponent(juce::AudioProcessorValueTreeState& apvts, int voiceIndex)
    : voiceEditor(apvts, juce::String(voiceIndex)),
      sequencer(8, voiceIndex, apvts)
    {
        addAndMakeVisible(voiceEditor);
        addAndMakeVisible(sequencer);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        auto voiceArea = area.removeFromTop(area.getHeight() * 0.65); // adjust ratio
        voiceEditor.setBounds(voiceArea);
        sequencer.setBounds(area);
    }

    VoiceEditor voiceEditor;
    StepSequencer sequencer;
};
