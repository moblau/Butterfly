#include "Filter.h"
#include <cmath>
#include <algorithm>
#include "ParamIDs.h"
Filter::Filter(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead) : DSPModule(apvtsRef,playHead){
    
}

Filter::~Filter() = default;

void Filter::prepare(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    halfSamplePeriod = 1.0 / (2.0 * sampleRate);
    for (auto& channelState : z)
        std::fill(channelState.begin(), channelState.end(), 0.0);
    
    adsr.setSampleRate(sampleRate);
    cutoffSmoothed.reset(sampleRate, 0.01); // 5 ms ramp
    cutoffSmoothed.setCurrentAndTargetValue(juce::jlimit(20.0, 20000.0, fc));
}

void Filter::reset(){
    for (auto& channelState : z)
        std::fill(channelState.begin(), channelState.end(), 0.0);
}

inline double mapDoubleValue(double& value, double min, double minMap, double slope)
{
    // --- bound to limits
    return minMap + slope * (value - min);
}

void Filter::computeCoeffs(double modulatedFc)
{
    double g = std::tan(juce::MathConstants<double>::pi * modulatedFc / sampleRate);
    alpha = g / (1.0 + g);
    alpha0 = 1.0 / (1.0 + K * std::pow(alpha, 4));
    beta[3] = 1.0 / (1.0 + g);
    beta[2] = alpha * beta[3];
    beta[1] = alpha * beta[2];
    beta[0] = alpha * beta[1];
}

void Filter::update()
{
    // tiny helper (same as before, just concise)
    auto pfloat = [this](const juce::String& id) -> float
    {
        if (auto* p = apvts.getRawParameterValue(id)) return *p;
        jassertfalse; return 0.0f;
    };

    // --- Drive (global) ---
    
    const bool freqModOn   = *apvts.getRawParameterValue("Frequency6modulate");
    const bool resonanceModOn   = *apvts.getRawParameterValue("Resonance6modulate");
    const bool envelopeModOn   = *apvts.getRawParameterValue("Envelope6modulate");
    const bool driveModOn   = *apvts.getRawParameterValue("Drive6modulate");

    const int currentStep = (int) pfloat(PID::seqCurrentStep[4]);
    const float stepValue = pfloat(PID::seqStep[4][juce::jlimit(0, 7, currentStep)]);
    
    const float rawDrive = pfloat(PID::filter_drive);
    driveParam = juce::jlimit(0.0f, 1.0f, rawDrive);

    constexpr float maxDriveDb = 36.0f;
    preGain  = juce::Decibels::decibelsToGain(driveParam * maxDriveDb);
    postGain = (preGain > 0.0f ? 1.0f / preGain : 1.0f);

    // --- Frequency/Resonance (global or per-voice lane) ---
    float freqParam = 22000.0f;
    float resParam  = 1.0f;

    if (usesEnvelope)
    {
        // Use global filter controls
        freqParam = pfloat(PID::filter_freq);
        resParam  = pfloat(PID::filter_res);
        if (freqModOn){
            static const juce::NormalisableRange<float> freqRange(20.0f, 22000.0f, 0.01f, 0.25f);

            float pos = freqRange.convertTo0to1(freqParam);
            // choose how “deep” the lane is in normalized units:
            const float depthNorm = 1.0f; // try 0.25f for subtler movement
            pos = juce::jlimit(0.0f, 1.0f, pos + stepValue * depthNorm);
            freqParam = freqRange.convertFrom0to1(pos);
        }
        if (resonanceModOn){
            resParam = juce::jlimit<float>(1,3,freqParam+stepValue*3);
        }
     
        
        // Envelope 1 settings (your code referenced attack1..release1)
        juce::ADSR::Parameters envParams;
        envParams.attack  = pfloat("attack1")  / 1000.0f;
        envParams.decay   = pfloat("decay1")   / 1000.0f;
        envParams.sustain = pfloat("sustain1");
        envParams.release = pfloat("release1") / 1000.0f;
        adsr.setParameters(envParams);
    }
    else
    {
        // Per-lane modulation: index is 1..4 in your strings; in arrays use 0..3
        const int i = juce::jlimit(1, 4, filterIndex) - 1;

        const bool modFreqActive = (pfloat(PID::FREQ_mod[i]) != 0.0f);
        const bool modResActive  = (pfloat(PID::RES_mod [i]) != 0.0f);

        // seq{index}CURRENT_STEP and corresponding step value
        const int seqIdx = juce::jlimit(0, 4, i); // map 0..3 → seq1..seq4 (0..3); adjust if you need seq5
        const int currentStep = (int) pfloat(PID::seqCurrentStep[seqIdx]);
        const float stepValue = pfloat(PID::seqStep[seqIdx][juce::jlimit(0, 7, currentStep)]);

        float filterSeq = 0.0f;
        if (modFreqActive)
            filterSeq = stepValue * 20000.0f;

        float resSeq = 0.0f;
        if (modResActive)
            resSeq = stepValue * 3.0f;

        freqParam = juce::jlimit(20.0f, 20000.0f, pfloat(PID::filter_freqN[i]) + filterSeq);
        resParam  = juce::jlimit(0.0f, 3.0f,      pfloat(PID::filter_resN [i]) + resSeq);
    }

    fc = freqParam;
    Q  = resParam;

    K = mapDoubleValue(Q, 1, 0, 2);

    // For the control-rate update() call, push target; per-sample smoothing happens in process()
    cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, (double) fc));

    // Optionally compute once here using the smoothed current value
    const double smoothFc = cutoffSmoothed.getNextValue();
    computeCoeffs(smoothFc);
}

//
//void Filter::update()
//{
//    
//    float rawDrive =  *apvts.getRawParameterValue("filter_drive");
//
//    driveParam = juce::jlimit(0.0f, 1.0f, rawDrive);
//
//    // Map 0..1 → 0 dB..+24 dB input gain (tweak max dB to taste)
//    constexpr float maxDriveDb = 36.0;
//    preGain  = juce::Decibels::decibelsToGain(driveParam * maxDriveDb);
//
//    // Small-signal unity gain compensation:
//    // slope of shaper near 0 ≈ preGain, so divide by preGain
//    postGain = (preGain > 0.0 ? 1.0 / preGain : 1.0);
//    
//    
//    float freqParam, resParam;
//
//    if ( usesEnvelope ){
//        freqParam = *apvts.getRawParameterValue("filter_freq");
//        resParam  = *apvts.getRawParameterValue("filter_res");
//        
//        auto* attackParam = apvts.getRawParameterValue("attack1");
//        auto* decayParam  = apvts.getRawParameterValue("decay1");
//        auto* sustainParam= apvts.getRawParameterValue("sustain1");
//        auto* releaseParam= apvts.getRawParameterValue("release1");
//        juce::ADSR::Parameters envParams;
//        envParams.attack  = *attackParam/1000;
//        envParams.decay   = *decayParam/1000;
//        envParams.sustain = *sustainParam;
//        envParams.release = *releaseParam/1000;
//
//        adsr.setParameters(envParams);
//    }
//    else{
//        bool modFreqActive = *apvts.getRawParameterValue("Frequency" + juce::String(filterIndex) + "modulate");
//        bool modResActive = *apvts.getRawParameterValue("Resonance" + juce::String(filterIndex) + "modulate");
//        
//        int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + juce::String(filterIndex) + "CURRENT_STEP"));
//        float stepValue = *apvts.getRawParameterValue("seq" + juce::String(filterIndex) + "step" + juce::String(currentStep));
//        
//        float filterSeq = 0;
//        if ( modFreqActive ){
//            filterSeq = stepValue*20000.0f;
//        }
//        float resSeq = 0;
//        if ( modResActive ){
//            resSeq = stepValue*3.0f;
//        }
//        freqParam = juce::jlimit(20.0f,20000.0f,*apvts.getRawParameterValue("filter_freq" + juce::String(filterIndex))+filterSeq);
//        resParam  = juce::jlimit(0.0f,3.0f,*apvts.getRawParameterValue("filter_res" + juce::String(filterIndex))+resSeq);
//
////        freqParam = *apvts.getRawParameterValue("filter_freq" + juce::String(filterIndex));
////        resParam  = *apvts.getRawParameterValue("filter_res" + juce::String(filterIndex));
//    }
//
//    fc = freqParam;
//    Q  = resParam;
////    numActivePoles =  juce::jlimit(1, maxPoles, static_cast<int>(*driveParam));
//
//    K = mapDoubleValue(Q, 1, 0, 2);
//
//    cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, static_cast<double>(fc)));
//    double smoothFc = cutoffSmoothed.getNextValue();  // <- smoothed value
//    computeCoeffs(smoothFc);
//
//
//}

//void Filter::update()
//{
//    auto* freqParam = apvts.getRawParameterValue("filter_freq");
//    auto* resParam  = apvts.getRawParameterValue("filter_res");
//    auto* driveParam = apvts.getRawParameterValue("filter_drive");
//    
//    auto* attackParam = apvts.getRawParameterValue("attack1");
//    auto* decayParam  = apvts.getRawParameterValue("decay1");
//    auto* sustainParam= apvts.getRawParameterValue("sustain1");
//    auto* releaseParam= apvts.getRawParameterValue("release1");
//    
//    fc = *freqParam;
//    Q  = *resParam;
////    numActivePoles =  juce::jlimit(1, maxPoles, static_cast<int>(*driveParam));
//
//    K = mapDoubleValue(Q, 1, 0, 2);
//
//    double g = std::tan(2.0 * juce::MathConstants<double>::pi * fc * halfSamplePeriod);
//    alpha = g / (1.0 + g);
//    alpha0 = 1.0 / (1.0 + K * std::pow(alpha, 4));
//
//    beta[3] = 1.0 / (1.0 + g);
//    beta[2] = alpha * beta[3];
//    beta[1] = alpha * beta[2];
//    beta[0] = alpha * beta[1];
//    
//    juce::ADSR::Parameters envParams;
//    envParams.attack  = *attackParam;
//    envParams.decay   = *decayParam;
//    envParams.sustain = *sustainParam;
//    envParams.release = *releaseParam;
//    
//    adsr.setParameters(envParams);
//    
//}
//
//void Filter::process(juce::AudioBuffer<float>& buffer)
//{
//    int numSamples = buffer.getNumSamples();
//    int numChannels = std::min(buffer.getNumChannels(), 2);
//
//    for (int ch = 0; ch < numChannels; ++ch)
//    {
//        float* channelData = buffer.getWritePointer(ch);
//        for (int i = 0; i < numSamples; ++i)
//        {
//            if (fc == 22000){
//                return;
//            }
//            if (usesEnvelope) {
//                double env = adsr.getNextSample();
//                float envMag = *apvts.getRawParameterValue("filter_env");
//                double targetFc = juce::jlimit(20.0, 20000.0, fc + env * envMag * 22000.0);
//                cutoffSmoothed.setTargetValue(targetFc);
//            } else {
//                cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, static_cast<double>(fc)));
//            }
//
//            double modFc = cutoffSmoothed.getNextValue();  // smoothed per-sample value
//            computeCoeffs(modFc);
//            
//            double input = channelData[i];
//            if (driveParam > 0.0f)
//            {
//                const float x = static_cast<float>(input * preGain);
//                const float y = softClip(x);        // or std::tanh(x)
//                input = static_cast<double>(y) * postGain;
//            }
//            
//            double sigma = 0.0;
//
//            for (int p = 0; p < numActivePoles; ++p)
//                sigma += z[ch][p]*beta[p];
//            
////            sigma = std::tanh(sigma);
//            input*= 1+K;
//            double u = alpha0 * (input - K * sigma);
//
//            for (int p = 0; p < numActivePoles; ++p)
//            {
//                u = z[ch][p] + alpha * (u - z[ch][p]);
//                double vn = (u-z[ch][p])*alpha;
//                z[ch][p] = u+vn;
//            }
//
//            channelData[i] = static_cast<float>(z[ch][numActivePoles - 1]);
//        }
//    }
//}

void Filter::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = std::min(buffer.getNumChannels(), 2);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            if (fc == 22000.0f)
                return;
            
//            const double fcNow = cutoffSmoothed.getNextValue(); // advance the ramp here
//                // Option A (cheapest): only recompute when fc changes “enough”
//            if (std::abs(fcNow - lastFc) > 0.1) {
//                computeCoeffs(fcNow);
//                lastFc = fcNow;
//            }
            
            // Per-sample target while envelope is active
            if (usesEnvelope)
            {
                const double env    = adsr.getNextSample();
                const float  envMag = *apvts.getRawParameterValue(PID::filter_env);
                const double target = juce::jlimit(20.0, 20000.0, (double) fc + env * envMag * 22000.0);
                cutoffSmoothed.setTargetValue(target);
            }
            else
            {
                // keep target pinned to last fc (already set in update()); harmless to set again
                cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, (double) fc));
            }

            const double modFc = cutoffSmoothed.getNextValue();
            computeCoeffs(modFc);

            double input = channelData[i];

            // Drive / pre-shaper gain + soft clip + post gain comp
            if (driveParam > 0.0f)
            {
                const float x = (float) (input * preGain);
                const float y = softClip(x);     // or std::tanh(x)
                input = (double) (y * postGain);
            }

            double sigma = 0.0;
            for (int p = 0; p < numActivePoles; ++p)
                sigma += z[ch][p] * beta[p];

            input *= (1.0 + K);
            double u = alpha0 * (input - K * sigma);

            for (int p = 0; p < numActivePoles; ++p)
            {
                u = z[ch][p] + alpha * (u - z[ch][p]);
                const double vn = (u - z[ch][p]) * alpha;
                z[ch][p] = u + vn;
            }

            channelData[i] = (float) z[ch][numActivePoles - 1];
        }
    }
}
