#include <JuceHeader.h>
#include "FMVoice.h"
#include "SequencerEngine.h"
class FMSynth : public juce::Synthesiser
{
public:
    FMSynth(juce::AudioProcessorValueTreeState& apvts);

    void updateSynthParameters();
    
    void syncVoices();
    
    FMVoice* getVoicePointer(int index)
    {
        return dynamic_cast<FMVoice*>(getVoice(index));
    }
    void setTimeInfo(float bpm, float ppq){ bpm = bpm; ppq = ppq;};
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    std::array<juce::String, 5> idSuffix;
    float bpm, ppq;
};
