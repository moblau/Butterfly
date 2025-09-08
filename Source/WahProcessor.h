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
    void updateParameters();
    void process(juce::AudioBuffer<float>& buffer) override;
    float processSample(float in, int channel);
    
    
    private:
    juce::dsp::DelayLine<float> delayLineReal;
    juce::dsp::DelayLine<float> delayLineImag;
    
    double sr = 44100.0;
    float lfoFrequency[2] = {0.0};
    float lfoStartPhase = 0.0f;
    float feedbackAmount = 0.75f;
    int delayLength = 20;
    
    float lfoPhase[2] = {0.0f};
    const int lfoSkipSamples = 25;
    std::complex<float> c;
    float channelLfoOffset[2] = {0};
};
