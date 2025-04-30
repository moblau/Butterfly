#include <JuceHeader.h>
#include "FMVoice.h"

class FMSynth : public juce::Synthesiser
{
public:
    void updateSynthParameters(juce::AudioProcessorValueTreeState& apvts, float stepValue);
    
    void syncVoices();
    
    FMVoice* getVoicePointer(int index)
    {
        return dynamic_cast<FMVoice*>(getVoice(index));
    }
};
