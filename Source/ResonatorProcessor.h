#pragma once
#include <JuceHeader.h>
#include <complex>
#include "DSPModule.h"


class ResonatorProcessor : public DSPModule
{
public:
    ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead);
    ~ResonatorProcessor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setDelayLines(const juce::MidiBuffer& midiMessages);

    
private:
    std::array<juce::dsp::DelayLine<float>, 4> delayLines;
    std::array<int, 4> activeMidiNotes;
    double sr;
    std::array<juce::dsp::IIR::Filter<float>, 4> dampingFilters;
    float ratio;
};
