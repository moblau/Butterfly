#include "Filter.h"
#include <cmath>
#include <algorithm>

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
    cutoffSmoothed.reset(sampleRate, 0.005); // 5 ms ramp
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
    
    float rawDrive =  *apvts.getRawParameterValue("filter_drive");

    driveParam = juce::jlimit(0.0f, 1.0f, rawDrive);

    // Map 0..1 → 0 dB..+24 dB input gain (tweak max dB to taste)
    constexpr float maxDriveDb = 36.0;
    preGain  = juce::Decibels::decibelsToGain(driveParam * maxDriveDb);

    // Small-signal unity gain compensation:
    // slope of shaper near 0 ≈ preGain, so divide by preGain
    postGain = (preGain > 0.0 ? 1.0 / preGain : 1.0);
    
    
    float freqParam, resParam;

    if ( usesEnvelope ){
        freqParam = *apvts.getRawParameterValue("filter_freq");
        resParam  = *apvts.getRawParameterValue("filter_res");
        
        auto* attackParam = apvts.getRawParameterValue("attack1");
        auto* decayParam  = apvts.getRawParameterValue("decay1");
        auto* sustainParam= apvts.getRawParameterValue("sustain1");
        auto* releaseParam= apvts.getRawParameterValue("release1");
        juce::ADSR::Parameters envParams;
        envParams.attack  = *attackParam/1000;
        envParams.decay   = *decayParam/1000;
        envParams.sustain = *sustainParam;
        envParams.release = *releaseParam/1000;

        adsr.setParameters(envParams);
    }
    else{
        bool modFreqActive = *apvts.getRawParameterValue("Frequency" + juce::String(filterIndex) + "modulate");
        bool modResActive = *apvts.getRawParameterValue("Resonance" + juce::String(filterIndex) + "modulate");
        
        int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + juce::String(filterIndex) + "CURRENT_STEP"));
        float stepValue = *apvts.getRawParameterValue("seq" + juce::String(filterIndex) + "step" + juce::String(currentStep));
        
        float filterSeq = 0;
        if ( modFreqActive ){
            filterSeq = stepValue*20000.0f;
        }
        float resSeq = 0;
        if ( modResActive ){
            resSeq = stepValue*3.0f;
        }
        freqParam = juce::jlimit(20.0f,20000.0f,*apvts.getRawParameterValue("filter_freq" + juce::String(filterIndex))+filterSeq);
        resParam  = juce::jlimit(0.0f,3.0f,*apvts.getRawParameterValue("filter_res" + juce::String(filterIndex))+resSeq);

//        freqParam = *apvts.getRawParameterValue("filter_freq" + juce::String(filterIndex));
//        resParam  = *apvts.getRawParameterValue("filter_res" + juce::String(filterIndex));
    }

    fc = freqParam;
    Q  = resParam;
//    numActivePoles =  juce::jlimit(1, maxPoles, static_cast<int>(*driveParam));

    K = mapDoubleValue(Q, 1, 0, 2);

    cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, static_cast<double>(fc)));
    double smoothFc = cutoffSmoothed.getNextValue();  // <- smoothed value
    computeCoeffs(smoothFc);


}

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

void Filter::process(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = std::min(buffer.getNumChannels(), 2);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            if (fc == 22000){
                return;
            }
            if (usesEnvelope) {
                double env = adsr.getNextSample();
                float envMag = *apvts.getRawParameterValue("filter_env");
                double targetFc = juce::jlimit(20.0, 20000.0, fc + env * envMag * 22000.0);
                cutoffSmoothed.setTargetValue(targetFc);
            } else {
                cutoffSmoothed.setTargetValue(juce::jlimit(20.0, 20000.0, static_cast<double>(fc)));
            }

            double modFc = cutoffSmoothed.getNextValue();  // smoothed per-sample value
            computeCoeffs(modFc);
            
            double input = channelData[i];
            if (driveParam > 0.0f)
            {
                const float x = static_cast<float>(input * preGain);
                const float y = softClip(x);        // or std::tanh(x)
                input = static_cast<double>(y) * postGain;
            }
            
            double sigma = 0.0;

            for (int p = 0; p < numActivePoles; ++p)
                sigma += z[ch][p]*beta[p];
            
//            sigma = std::tanh(sigma);
            input*= 1+K;
            double u = alpha0 * (input - K * sigma);

            for (int p = 0; p < numActivePoles; ++p)
            {
                u = z[ch][p] + alpha * (u - z[ch][p]);
                double vn = (u-z[ch][p])*alpha;
                z[ch][p] = u+vn;
            }

            channelData[i] = static_cast<float>(z[ch][numActivePoles - 1]);
        }
    }
}
