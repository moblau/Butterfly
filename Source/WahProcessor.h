#pragma once
#include <JuceHeader.h>
#include <complex>

class WahProcessor
{
public:
    WahProcessor();
    void prepare(double sampleRate, int samplesPerBlock, int maxDelayInSamples);
    void reset();
    void setParameters(float lfoFreq, float startPhase, float feedbackAmount, int delaySamples);
    void process(juce::AudioBuffer<float>& buffer);

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
