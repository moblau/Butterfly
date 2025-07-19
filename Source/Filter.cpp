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
        freqParam = *apvts.getRawParameterValue("filter_freq" + juce::String(filterIndex));
        resParam  = *apvts.getRawParameterValue("filter_res" + juce::String(filterIndex));

    }

    fc = freqParam;
    Q  = resParam;
//    numActivePoles =  juce::jlimit(1, maxPoles, static_cast<int>(*driveParam));

    K = mapDoubleValue(Q, 1, 0, 2);

    computeCoeffs(fc);



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
            if (usesEnvelope){
                double envValue = adsr.getNextSample();
                double modulatedFc = fc;
                if (i % 8 == 0)
                        computeCoeffs(modulatedFc);
            }
            else{
                computeCoeffs(fc);
            }
            
            double input = channelData[i];
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
