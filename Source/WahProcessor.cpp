#include "WahProcessor.h"

WahProcessor::WahProcessor() {}

void WahProcessor::prepare(double sampleRate, int samplesPerBlock, int maxDelayInSamples)
{
    sr = sampleRate;
    delayLineReal.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineReal.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineImag.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineImag.setMaximumDelayInSamples(maxDelayInSamples);
}

void WahProcessor::reset()
{
    delayLineReal.reset();
    delayLineImag.reset();
    lfoPhase = 0.0f;
}

void WahProcessor::setParameters(float lfoFreq, float startPhase, float feedback, int delaySamples)
{
    lfoFrequency = lfoFreq;
    lfoStartPhase = startPhase;
    feedbackAmount = feedback;
    delayLength = delaySamples;
}

void WahProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const float phaseIncrement = lfoFrequency / sr;

    for (int i = 0; i < numSamples; ++i)
    {
        float in = buffer.getSample(0, i);
        c = std::polar(1.0f, lfoPhase * 2.0f * juce::MathConstants<float>::pi);
        float re = (in + feedbackAmount * delayLineReal.popSample(0)) * c.real();
        float im = (in + feedbackAmount * delayLineImag.popSample(0)) * c.imag();
        delayLineReal.pushSample(0, re);
        delayLineImag.pushSample(0, im);
        buffer.setSample(0, i, re);
        lfoPhase += phaseIncrement;
        if (lfoPhase > 1.0f) lfoPhase -= 1.0f;
    }
}
