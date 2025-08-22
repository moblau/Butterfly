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
    
    void startADSR() { adsr.noteOn(); }
    void stopADSR() { adsr.noteOff(); }
    void computeCoeffs(double modulatedFc);
    
    void setEnvelopeStatus(bool newStatus){
        usesEnvelope = newStatus;
    }
    void setVoiceNum(int voiceIndex){
        filterIndex = voiceIndex;
    }
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
    
    bool usesEnvelope;
    int filterIndex;
    std::array<std::array<double, maxPoles>, 2> z = {};
    std::array<double, maxPoles> beta = {};
    
    juce::ADSR adsr;
    
    void calcCoefficients();
    
    // In Filter class (private:)
    float driveParam = 0.0f;     // 0..1 from the UI
    double preGain = 1.0;        // input gain before shaper
    double postGain = 1.0;       // makeup to keep small-signal unity

    static inline float softClip(float x) noexcept
    {
        // Smooth, cheap, good-sounding. Swap for std::tanh(x) if you prefer.
        // Cubic soft clip (odd polynomial), ~tanh-like curve:
        const float a = juce::jlimit(-2.0f, 2.0f, x);
        return a - (a * a * a) / 3.0f;
    }
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> cutoffSmoothed;
    float lastFc;
};
