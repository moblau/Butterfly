#pragma once
#include <JuceHeader.h>
#include <complex>
#include "DSPModule.h"
#include "ParamIDs.h"
class WahProcessor : public DSPModule
{
public:
    WahProcessor(juce::AudioProcessorValueTreeState& apvtsRef, juce::AudioPlayHead* playHead);
    ~WahProcessor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void setParameters(float lfoFreq, float startPhase, float feedbackAmount, int delaySamples);
    void process(juce::AudioBuffer<float>& buffer) override;


    
private:
    juce::dsp::DelayLine<float> delayLineReal;
    juce::dsp::DelayLine<float> delayLineImag;

    double sr = 44100.0;
    float lfoFrequency = 0.6f;
    float lfoStartPhase = 0.0f;
    float feedbackAmount = 0.75f;
    int delayLength = 20;

    float lfoPhase = 0.0f;
    const int lfoSkipSamples = 25;
    std::complex<float> c;
};
