#pragma once
#include <array>
#include "DSPModule.h"

class Filter : public DSPModule
{
public:
    Filter(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead);
    ~Filter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void update(); // poles = 1 to 4
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
private:
    static constexpr int maxPoles = 4;

    double sampleRate = 44100.0;
    double halfSamplePeriod = 1.0 / (2.0 * 44100.0);

    double fc = 22000.0; // Hz
    double Q = 1.0;     // Resonance
    int numActivePoles = 4;

    double K = 0.0;
    double alpha = 0.0;
    double alpha0 = 1.0;

    std::array<std::array<double, maxPoles>, 2> z = {};
    std::array<double, maxPoles> beta = {};

    void calcCoefficients();
};
