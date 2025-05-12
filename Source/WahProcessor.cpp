#include "WahProcessor.h"

WahProcessor::WahProcessor(juce::AudioProcessorValueTreeState& apvtsRef) : DSPModule(apvtsRef) {}

WahProcessor::~WahProcessor() {}


void WahProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    delayLineReal.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineReal.setMaximumDelayInSamples(50);
    delayLineImag.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineImag.setMaximumDelayInSamples(50);
    setParameters(10, 0.0f, 0.9f, 50);
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
    float freq = *apvts.getRawParameterValue("wahFreq");
    float feedback = *apvts.getRawParameterValue("wahFeedback");
    float delay = *apvts.getRawParameterValue("wahDelay");
    DBG(freq);
    // Optional: clamp or convert if needed
    int delaySamples = static_cast<int>(delay);

    // Call setParameters
    setParameters(freq, 0.0f, feedback, delaySamples);
    
    const int numSamples = buffer.getNumSamples();
    const float twoPi = juce::MathConstants<float>::twoPi;
    const float phaseIncrement = lfoFrequency *twoPi/ sr;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (sample % lfoSkipSamples == 0)
        {
            float lfo = 1.0f + std::cos(lfoPhase + lfoStartPhase);
            c = std::complex<float>(std::cos(lfo) * feedbackAmount,
                                    std::sin(lfo) * feedbackAmount);
        }

        for (int channel = 0; channel < 2; ++channel)
        {
            float inputSample = buffer.getSample(channel, sample);

            // First read old delayed samples
            float delayedReal = delayLineReal.popSample(0); // tap 0 = now - delay
            float delayedImag = delayLineImag.popSample(0);

            std::complex<float> delayed(delayedReal, delayedImag);
            std::complex<float> input(inputSample, 0.0f);

            auto outComplex = c * delayed + (1.0f - feedbackAmount) * input;

            // NOW set the delay time before writing
            delayLineReal.setDelay(delayLength);
            delayLineImag.setDelay(delayLength);

            // Then push new samples
            delayLineReal.pushSample(0, outComplex.real());
            delayLineImag.pushSample(0, outComplex.imag());

            float outputSample = outComplex.real() * 3.0f;
            outputSample = juce::jlimit(-32768.0f, 32767.0f, outputSample);

            buffer.setSample(channel, sample, outputSample);
        }

        lfoPhase += phaseIncrement;
        if (lfoPhase > twoPi)
            lfoPhase -= twoPi;
    }
    
//    for (int i = 0; i < numSamples; ++i)
//    {
//        float in = buffer.getSample(0, i);
//        c = std::polar(1.0f, lfoPhase * 2.0f * juce::MathConstants<float>::pi);
//        float re = (in + feedbackAmount * delayLineReal.popSample(0)) * c.real();
//        float im = (in + feedbackAmount * delayLineImag.popSample(0)) * c.imag();
//        delayLineReal.pushSample(0, re);
//        delayLineImag.pushSample(0, im);
//        buffer.setSample(0, i, re);
//        lfoPhase += phaseIncrement;
//        if (lfoPhase > 1.0f) lfoPhase -= 1.0f;
//    }
}
