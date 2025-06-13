#pragma once
#include <JuceHeader.h>
#include "FMSound.h"
#include "SmoothDownsampler.h"
class FMVoice : public juce::SynthesiserVoice
{
public:
    FMVoice();
    
    void prepare(double sampleRate, int samplesPerBlock);
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setWaveform(int waveformId);
    void setModulatorWaveform(int waveformId);
    void setModulationRatio(int numerator, int denominator);
    void setModulationIndex(float newIndex);
    void setDetune(float semitoneOffset);
    void setPan(float newPan);
    void setDownsampleFactor(int dsFactor);

    float getCarrierPhase() const;
    void resetCarrierPhase();
    float getModSignal() const;
    void resetModPhase();
    bool getPhaseReset();
    
    void setGain(float g);
    
    void setVoiceId(int id) { voiceId = id; }
    int getVoiceId() const { return voiceId; }
    void setAlias(bool newAliasStaate);
    void setEnvelopeParams(const juce::ADSR::Parameters& newParams);

private:
    int voiceId = -1;
    enum class Waveform { Sine, Saw, Square };
    float getSample(float phase, Waveform wf, bool bandLimited, float dt);
    float polyBLEP(float t, float dt);

    Waveform modWaveform = Waveform::Sine;
    Waveform waveform = Waveform::Sine;

    float modSignal = 0;
    float phase = 0.0f, modPhase = 0.0f;
    float carrierFrequency = 0.0f, modulatorFrequency = 0.0f;
    float level = 0.0f;
    float baseFrequency = 0.0f;
    float detuneRatio = 1.0f;
    float modulationIndex = 2.0f;
    float modulationRatio = 3.0f;
    int modRatioNum = 3;
    int modRatioDen = 1;
    float dsFactor;
    float pan = 0.5f;
    float gain = 1.0f;
    SmoothDownsampler ds;
    bool phaseReset = false;
    const float twoPi = juce::MathConstants<float>::twoPi;
    float prevWarpedPhase;
    int midiNoteNumber;
    
    
    bool shouldAlias = false;
    juce::ADSR       env;
    juce::ADSR::Parameters envParams;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
};
