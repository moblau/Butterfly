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
}

void Filter::reset(){
}

void Filter::update()
{
    auto* freqParam = apvts.getRawParameterValue("filter_freq");
    auto* resParam  = apvts.getRawParameterValue("filter_res");
    auto* driveParam = apvts.getRawParameterValue("filter_drive");
    
    fc = *freqParam;
    Q  = *resParam;
    numActivePoles =  juce::jlimit(1, maxPoles, static_cast<int>(*driveParam));

    K = Q;

    double g = std::tan(2.0 * juce::MathConstants<double>::pi * fc * halfSamplePeriod);
    alpha = g / (1.0 + g);
    alpha0 = 1.0 / (1.0 + K * std::pow(alpha, 4));

    beta[3] = 1.0 / (1.0 + g);
    beta[2] = alpha * beta[3];
    beta[1] = alpha * beta[2];
    beta[0] = alpha * beta[1];
}

void Filter::process(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = std::min(buffer.getNumChannels(), 2);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            double input = channelData[i];
            double sigma = 0.0;

            for (int p = 0; p < numActivePoles; ++p)
                sigma += z[ch][p];
            sigma = std::tanh(sigma);
            double u = alpha0 * (input - K * sigma);

            for (int p = 0; p < numActivePoles; ++p)
            {
                u = z[ch][p] + alpha * (u - z[ch][p]);
                z[ch][p] = u;
            }

            channelData[i] = static_cast<float>(z[ch][numActivePoles - 1]);
        }
    }
}
