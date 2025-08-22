#include "WahProcessor.h"

WahProcessor::WahProcessor(juce::AudioProcessorValueTreeState& apvtsRef, juce::AudioPlayHead* playHead) : DSPModule(apvtsRef, playHead){}

WahProcessor::~WahProcessor() {}


void WahProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    delayLineReal.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineReal.setMaximumDelayInSamples(100);
    delayLineImag.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 1 });
    delayLineImag.setMaximumDelayInSamples(100);
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
    auto pfloat = [this](const juce::String& id) -> float
    {
        if (auto* p = apvts.getRawParameterValue(id)) return *p;
        jassertfalse; return 0.0f;
    };
    
    // 1) Read “free‐running” parameters:
    float freeFreq   = *apvts.getRawParameterValue("wahFreq");     // in Hz
    float feedback   = *apvts.getRawParameterValue("wahFeedback"); // 0…1
    float delay      = *apvts.getRawParameterValue("wahDelay");    // in samples (or however you store it)
    
    const bool freqModOn   = *apvts.getRawParameterValue("Frequency5modulate");
    const bool feedbackModOn   = *apvts.getRawParameterValue("Feedback5modulate");
    const bool delayModOn   = *apvts.getRawParameterValue("Delay5modulate");

    const int currentStep = (int) pfloat(PID::seqCurrentStep[4]);
    const float stepValue = pfloat(PID::seqStep[4][juce::jlimit(0, 7, currentStep)]);
    
    // 2) Read the new “sync” parameters:
    bool  useSync    = static_cast<bool>(*apvts.getRawParameterValue("WahUseSync"));
    int   syncIndex  = static_cast<int>(*apvts.getRawParameterValue("WahSyncRate")); // 1=“1/4”, 2=“1/8”, etc.

    // 3) If syncing, pull host BPM and calculate LFO frequency:
    float lfoFreqToUse = freeFreq; // default
    if (useSync)
    {
        // Ask the host for its current transport info:
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (auto* ph = playHead)
            ph->getCurrentPosition(posInfo);

        float bpm = static_cast<float>(posInfo.bpm);
        if (bpm <= 0.0f)
            bpm = 120.0f; // fallback if host didn’t provide a valid BPM

        // Convert syncIndex → division:
        //   1 → “1/4”  → one LFO cycle per quarter note  → freq = bpm / 60
        //   2 → “1/8”  → one LFO cycle per eighth note   → freq = bpm / 30
        //   3 → “1/16” → one LFO cycle per sixteenth note→ freq = bpm / 15
        //   4 → “1/32” → one LFO cycle per thirty‐second→ freq = bpm / 7.5
//        DBG(syncIndex);
//        switch (syncIndex)
//        {
//            case 0: lfoFreqToUse = bpm / (60.0f * (1.0f / 32.0f)); break; // 1/32
//            case 1: lfoFreqToUse = bpm / (60.0f * (1.0f / 16.0f)); break; // 1/16
//            case 2: lfoFreqToUse = bpm / (60.0f * (1.0f / 8.0f));  break; // 1/8
//            case 3: lfoFreqToUse = bpm / (60.0f * (1.0f / 4.0f));  break; // 1/4
//            case 4: lfoFreqToUse = bpm / (60.0f * (1.0f / 2.0f));  break; // 1/2
//            case 5: lfoFreqToUse = bpm / 60.0f;                    break; // 1/1
//            case 6: lfoFreqToUse = bpm / (60.0f / 2.0f);           break; // 2/1
//            case 7: lfoFreqToUse = bpm / (60.0f / 3.0f);           break; // 3/1
//            case 8: lfoFreqToUse = bpm / (60.0f / 4.0f);           break; // 4/1
//            default: lfoFreqToUse = bpm / 60.0f;                   break; // fallback: 1 beat per second
//        }
        
        std::array<float, 9> beatsPerCycle = {
            0.125f, 0.25f, 0.5f, 1.0f,
            2.0f, 4.0f, 8.0f, 12.0f, 16.0f
        };

        if (syncIndex >= 0 && syncIndex < (int)beatsPerCycle.size())
            lfoFreqToUse = (bpm / 60.0f) / beatsPerCycle[syncIndex];
        else
            lfoFreqToUse = bpm / 60.0f;
        
        
        
    }
    else
    {
        if (freqModOn){
            lfoFreqToUse = juce::jlimit<float>(1.0,10.0,freeFreq+stepValue*50.0);
        }
        
    }
    // 4) Convert “delay” to an integer number of samples (clamp if needed):
    int delaySamples = juce::jlimit(5,  100,  static_cast<int>(delay));
    
    if( delayModOn){
        delaySamples = juce::jlimit(5,  100,  static_cast<int>(delay+stepValue*100.0));
    }
    
    if (feedbackModOn)
    {
        feedback = juce::jlimit<float>(0,1.0,feedback+stepValue);
    }
    // 5) Update our internal LFO frequency, feedback, and delay‐length:
    setParameters(lfoFreqToUse, 0.0f, feedback, delaySamples);

    // --- Now run the usual per‐sample loop, using updated lfoFrequency, feedbackAmount, delayLength ---
    const int numSamples   = buffer.getNumSamples();
    const float twoPi      = juce::MathConstants<float>::twoPi;
    const float phaseIncr  = lfoFrequency * twoPi / static_cast<float>(sr);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Only update c (our complex coefficient) every lfoSkipSamples for efficiency:
        if (sample % lfoSkipSamples == 0)
        {
            float lfoValue = 1.0f + std::cos(lfoPhase + lfoStartPhase);
            c = std::complex<float>(
                  std::cos(lfoValue) * feedbackAmount,
                  std::sin(lfoValue) * feedbackAmount
            );
        }

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float in = buffer.getSample(channel, sample);

            float delayedReal = delayLineReal.popSample(0);
            float delayedImag = delayLineImag.popSample(0);
            std::complex<float> delayed(delayedReal, delayedImag);
            std::complex<float> input(in, 0.0f);

            auto outComplex = c * delayed + (1.0f - feedbackAmount) * input;

            // Set the new delay length, then push current sample:
            delayLineReal.setDelay(delayLength);
            delayLineImag.setDelay(delayLength);

            delayLineReal.pushSample(0, outComplex.real());
            delayLineImag.pushSample(0, outComplex.imag());

            float out = outComplex.real() * 3.0f;
            out = juce::jlimit(-32768.0f, 32767.0f, out);

            buffer.setSample(channel, sample, out);
        }

        lfoPhase += phaseIncr;
        if (lfoPhase >= twoPi)
            lfoPhase -= twoPi;
    }
}
//void WahProcessor::process(juce::AudioBuffer<float>& buffer)
//{
//    float freq = *apvts.getRawParameterValue("wahFreq");
//    float feedback = *apvts.getRawParameterValue("wahFeedback");
//    float delay = *apvts.getRawParameterValue("wahDelay");
//    DBG(freq);
//    // Optional: clamp or convert if needed
//    int delaySamples = static_cast<int>(delay);
//
//    // Call setParameters
//    setParameters(freq, 0.0f, feedback, delaySamples);
//    
//    const int numSamples = buffer.getNumSamples();
//    const float twoPi = juce::MathConstants<float>::twoPi;
//    const float phaseIncrement = lfoFrequency *twoPi/ sr;
//
//    for (int sample = 0; sample < numSamples; ++sample)
//    {
//        if (sample % lfoSkipSamples == 0)
//        {
//            float lfo = 1.0f + std::cos(lfoPhase + lfoStartPhase);
//            c = std::complex<float>(std::cos(lfo) * feedbackAmount,
//                                    std::sin(lfo) * feedbackAmount);
//        }
//
//        for (int channel = 0; channel < 2; ++channel)
//        {
//            float inputSample = buffer.getSample(channel, sample);
//
//            // First read old delayed samples
//            float delayedReal = delayLineReal.popSample(0); // tap 0 = now - delay
//            float delayedImag = delayLineImag.popSample(0);
//
//            std::complex<float> delayed(delayedReal, delayedImag);
//            std::complex<float> input(inputSample, 0.0f);
//
//            auto outComplex = c * delayed + (1.0f - feedbackAmount) * input;
//
//            // NOW set the delay time before writing
//            delayLineReal.setDelay(delayLength);
//            delayLineImag.setDelay(delayLength);
//
//            // Then push new samples
//            delayLineReal.pushSample(0, outComplex.real());
//            delayLineImag.pushSample(0, outComplex.imag());
//
//            float outputSample = outComplex.real() * 3.0f;
//            outputSample = juce::jlimit(-32768.0f, 32767.0f, outputSample);
//
//            buffer.setSample(channel, sample, outputSample);
//        }
//
//        lfoPhase += phaseIncrement;
//        if (lfoPhase > twoPi)
//            lfoPhase -= twoPi;
//    }
//    
////    for (int i = 0; i < numSamples; ++i)
////    {
////        float in = buffer.getSample(0, i);
////        c = std::polar(1.0f, lfoPhase * 2.0f * juce::MathConstants<float>::pi);
////        float re = (in + feedbackAmount * delayLineReal.popSample(0)) * c.real();
////        float im = (in + feedbackAmount * delayLineImag.popSample(0)) * c.imag();
////        delayLineReal.pushSample(0, re);
////        delayLineImag.pushSample(0, im);
////        buffer.setSample(0, i, re);
////        lfoPhase += phaseIncrement;
////        if (lfoPhase > 1.0f) lfoPhase -= 1.0f;
////    }
//}
