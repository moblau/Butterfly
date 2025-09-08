#pragma once
#include <JuceHeader.h>
#include <complex>
#include "DSPModule.h"
#include "ParamIDs.h"

class ResonatorProcessor : public DSPModule
{
public:
    ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead, int res_index);
    ~ResonatorProcessor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setDelayLines(int freq);
    float processSample (float input, int channel);
    
    void setStepValue(float sv) { stepValue = sv;};
    void updateParameters();
private:
    float offsetNorm = 0.0f;
    float feedbackAmount = 0.0f;
    float offsetStrength = 0.0f;
    float stepValue = 0.0f;

    bool modResOffsetActive = false;
    bool modResFeedbackActive = false;
    bool modResOffsetStrengthActive = false;
    
//    std::array<juce::dsp::DelayLine<float>, 4> delayLines;
    juce::dsp::DelayLine<float> delayLine;
    int delayInSamples;
//    std::array<int, 4> activeMidiNotes;
    int activeMidiNote;
    double sr;
    int res_index;
//    std::array<juce::dsp::IIR::Filter<float>, 4> dampingFilters;
//    float ratio;
};
