

#include "ResonatorProcessor.h"

ResonatorProcessor::ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead, int res_index) : DSPModule(apvtsRef,playHead), res_index(juce::String(res_index)) {}
ResonatorProcessor::~ResonatorProcessor() = default;

void ResonatorProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;                        // e.g. 44100.0
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = 2;

    delayLine.prepare(spec);
    delayLine.setMaximumDelayInSamples(sr*4);

}

void ResonatorProcessor::reset(){
}

void ResonatorProcessor::setDelayLines(int freq)
{
    delayInSamples = sr/freq;
    delayLine.setDelay(delayInSamples);
}

void ResonatorProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int totalNumSamples  = buffer.getNumSamples();
    const int totalNumChannels = buffer.getNumChannels();

    float offsetNorm = *apvts.getRawParameterValue("res_offset" + res_index);
    float feedbackAmount      = *apvts.getRawParameterValue("res_feedback" + res_index);
    
    bool modResOffsetActive = *apvts.getRawParameterValue("Resonator Offset" + juce::String(res_index) + "modulate");
    bool modResFeedbackActive = *apvts.getRawParameterValue("Resonator Feedback" + juce::String(res_index) + "modulate");
    
    int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + juce::String(res_index) + "CURRENT_STEP"));
    float stepValue = *apvts.getRawParameterValue("seq" + juce::String(res_index) + "step" + juce::String(currentStep));
    
    float resFeedbackSeq = 0;
    if ( modResFeedbackActive ){
        feedbackAmount += juce::jlimit(0.0f,1.0f,stepValue);
    }
    
    float resOffsetSeq = 0;
    if ( modResOffsetActive ){
        offsetNorm += juce::jlimit(0.0f,1.0f,stepValue);
    }
    float offsetHz = (offsetNorm * 2.0f - 1.0f) * 1000.0f;

    // Compute base frequency from the original delay
    float baseFreq = sr / delayInSamples;
    float newFreq = juce::jlimit<float>(20.0f, sr / 2.0f - 1.0f, baseFreq + offsetHz);
    
    // Convert back to delay in samples
    float newDelayInSamples = sr / newFreq;

    delayLine.setDelay(newDelayInSamples);
    for (int sample = 0; sample < totalNumSamples; ++sample)
    {
        float inputL = buffer.getSample(0, sample);
        float inputR = buffer.getSample(1, sample);

        //        float inputR = (totalNumChannels > 1) ? buffer.getSample(1, sample) : inputL;

        float r_delay = delayLine.popSample(0);
        float l_delay = delayLine.popSample(1);
        
        float out_l = inputL + r_delay*feedbackAmount;
        float out_r = inputR + l_delay*feedbackAmount;
        
        delayLine.pushSample(0,out_l);
        delayLine.pushSample(1,out_r);
        buffer.setSample(0, sample, out_l);
        buffer.setSample(1, sample, out_r);
    }
}
