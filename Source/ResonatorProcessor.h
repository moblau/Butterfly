#pragma once
#include <JuceHeader.h>
#include <complex>
#include "DSPModule.h"


class ResonatorProcessor : public DSPModule
{
public:
    ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead, int res_index);
    ~ResonatorProcessor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setDelayLines(int freq);

    
private:
//    std::array<juce::dsp::DelayLine<float>, 4> delayLines;
    juce::dsp::DelayLine<float> delayLine;
    int delayInSamples;
//    std::array<int, 4> activeMidiNotes;
    int activeMidiNote;
    double sr;
    juce::String res_index;
//    std::array<juce::dsp::IIR::Filter<float>, 4> dampingFilters;
//    float ratio;
};
