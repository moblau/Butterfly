

#include "ResonatorProcessor.h"

ResonatorProcessor::ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead, int res_index) : DSPModule(apvtsRef,playHead), res_index(res_index-1) {}
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
    
    float offsetNorm        = *apvts.getRawParameterValue(PID::res_offset[res_index]);
    float feedbackAmount    = *apvts.getRawParameterValue(PID::res_feedback[res_index]);
    float offsetStrength    = *apvts.getRawParameterValue(PID::offset_strength[res_index]); // <-- if you have a separate array for strength, replace here
//
    bool modResOffsetActive       = *apvts.getRawParameterValue(PID::RESOFF_mod[res_index]);
    bool modResFeedbackActive     = *apvts.getRawParameterValue(PID::RESFDB_mod[res_index]);
    bool modResOffsetStrengthActive = *apvts.getRawParameterValue(PID::OFFSTR_mod[res_index]); // same note: add an `OFFSET_STRENGTH_mod` array if needed

//    float offsetNorm        = 0;
//    float feedbackAmount    = 0;
// 
//    bool modResOffsetActive       = 0;
//    bool modResFeedbackActive     = 0;
//    bool modResOffsetStrengthActive = 0;
    
    int currentStep = static_cast<int>(*apvts.getRawParameterValue(PID::seqCurrentStep[res_index]));
    float stepValue = *apvts.getRawParameterValue(PID::seqStep[res_index][currentStep]);
//    float offsetNorm = *apvts.getRawParameterValue("res_offset" + res_index);
//    float feedbackAmount      = *apvts.getRawParameterValue("res_feedback" + res_index);
//    float offsetStrength      = *apvts.getRawParameterValue("res_offset_strength" + res_index);
//
//    
//    bool modResOffsetActive = *apvts.getRawParameterValue("Resonator Offset" + juce::String(res_index) + "modulate");
//    bool modResFeedbackActive = *apvts.getRawParameterValue("Resonator Feedback" + juce::String(res_index) + "modulate");
//    bool modResOffsetStrengthActive = *apvts.getRawParameterValue("Offset Strength" + juce::String(res_index) + "modulate");
//    
//    int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + juce::String(res_index) + "CURRENT_STEP"));
//    float stepValue = *apvts.getRawParameterValue("seq" + juce::String(res_index) + "step" + juce::String(currentStep));
    
//    float resOffsetSeq = 0;
    if ( modResOffsetStrengthActive ){
        offsetStrength += juce::jlimit(0.0f,1.0f,stepValue);
    }
//    float resFeedbackSeq = 0;
    if ( modResFeedbackActive ){
        feedbackAmount += juce::jlimit(0.0f,1.0f,stepValue);
    }
    
//    float resOffsetSeq = 0;
    if ( modResOffsetActive ){
        offsetNorm += juce::jlimit(0.0f,1.0f,stepValue);
    }
    float offsetHz = (offsetNorm * 2.0f - 1.0f) * 1000.0f*offsetStrength;

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
float ResonatorProcessor::processSample(float input, int channel)
{
    float offsetHz = (offsetNorm * 2.0f - 1.0f) * 1000.0f * offsetStrength;

    float baseFreq = sr / delayInSamples;
    float newFreq  = juce::jlimit<float>(20.0f, sr * 0.5f - 1.0f, baseFreq + offsetHz);
    float newDelayInSamples = sr / newFreq;

    delayLine.setDelay(newDelayInSamples);

    float delayed = delayLine.popSample(channel);
    float out     = input + delayed * feedbackAmount;

    delayLine.pushSample(channel, out);
    return out;
}

//
//float ResonatorProcessor::processSample (float input, int channel)
//{
//    // (You’ll want to store these in member vars instead of fetching from apvts each time if possible)
//    float offsetNorm        = *apvts.getRawParameterValue(PID::res_offset[res_index]);
//    float feedbackAmount    = *apvts.getRawParameterValue(PID::res_feedback[res_index]);
//    float offsetStrength    = *apvts.getRawParameterValue(PID::offset_strength[res_index]);
//
//    bool modResOffsetActive        = *apvts.getRawParameterValue(PID::RESOFF_mod[res_index]);
//    bool modResFeedbackActive      = *apvts.getRawParameterValue(PID::RESFDB_mod[res_index]);
//    bool modResOffsetStrengthActive= *apvts.getRawParameterValue(PID::OFFSTR_mod[res_index]);
//
////    int currentStep = static_cast<int>(*apvts.getRawParameterValue(PID::seqCurrentStep[res_index]));
////    float stepValue = *apvts.getRawParameterValue(PID::seqStep[res_index][currentStep]);
//
//    // Apply modulation
//    if (modResOffsetStrengthActive)
//        offsetStrength += juce::jlimit(0.0f, 1.0f, stepValue);
//
//    if (modResFeedbackActive)
//        feedbackAmount += juce::jlimit(0.0f, 1.0f, stepValue);
//
//    if (modResOffsetActive)
//        offsetNorm += juce::jlimit(0.0f, 1.0f, stepValue);
//
//    float offsetHz = (offsetNorm * 2.0f - 1.0f) * 1000.0f * offsetStrength;
//
//    // Compute frequency/delay
//    float baseFreq = sr / delayInSamples;
//    float newFreq = juce::jlimit<float>(20.0f, sr / 2.0f - 1.0f, baseFreq + offsetHz);
//    float newDelayInSamples = sr / newFreq;
//
//    delayLine.setDelay(newDelayInSamples);
//
//    // Process one channel
//    float delayed = delayLine.popSample(channel);
//    float out = input + delayed * feedbackAmount;
//
//    delayLine.pushSample(channel, out);
//    return out;
//}

void ResonatorProcessor::updateParameters()
{
    offsetNorm     = *apvts.getRawParameterValue(PID::res_offset[res_index]);
    feedbackAmount = *apvts.getRawParameterValue(PID::res_feedback[res_index]);
    offsetStrength = *apvts.getRawParameterValue(PID::offset_strength[res_index]);

    modResOffsetActive         = *apvts.getRawParameterValue(PID::RESOFF_mod[res_index]);
    modResFeedbackActive       = *apvts.getRawParameterValue(PID::RESFDB_mod[res_index]);
    modResOffsetStrengthActive = *apvts.getRawParameterValue(PID::OFFSTR_mod[res_index]);

    int currentStep = static_cast<int>(*apvts.getRawParameterValue(PID::seqCurrentStep[res_index]));
    stepValue       = *apvts.getRawParameterValue(PID::seqStep[res_index][currentStep]);

    // Apply modulation
    if (modResOffsetStrengthActive)
        offsetStrength = juce::jlimit(0.0f, 1.0f, offsetStrength + stepValue);

    if (modResFeedbackActive)
        feedbackAmount = juce::jlimit(0.0f, .95f, feedbackAmount + stepValue);

    if (modResOffsetActive)
        offsetNorm = juce::jlimit(0.0f, 1.0f, offsetNorm + stepValue);
}
