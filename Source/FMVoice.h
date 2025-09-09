#pragma once
#include <JuceHeader.h>
#include "FMSound.h"
#include "SmoothDownsampler.h"
#include "Filter.h"
#include "ResonatorProcessor.h"
#include "SequencerEngine.h"
#include "ParamIDs.h"

struct VoiceParams
{
    float detune;
    float pan;
    float downsample;
    float modIndex;
    float modNum;
    float modDen;
    float carrierWaveform;
    float modulatorWaveform;
    bool alias;
    float selfModAmt;
    float bpm;
    float ppq;
    int octave;
    float vibFreq;
    float vibDepth;
    
    bool modPanOn       = false;
    bool modDetuneOn    = false;
    bool modAmountOn    = false;   // FM index
    bool modNumOn       = false;   // ratio numerator
    bool modDenOn       = false;   // ratio denominator
    bool modDownsampleOn= false;
    bool modOctaveOn = false;
    bool modVibFreqOn = false;
    bool modVibDepthOn = false;
    
};
class FMVoice : public juce::SynthesiserVoice
{
public:
    FMVoice(juce::AudioProcessorValueTreeState &apvtsRef,juce::AudioPlayHead* playHead, int voiceNum, int osFactor);
    
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

    int   getMidiNote()          const { return midiNoteNumber; }
    float getCarrierFrequency()  const { return carrierFrequency; }
    float getModulationRatio()   const { return modulationRatio; }   // carrier->mod ratio
    int   getModWaveform()       const { return (int)modWaveform; }
    float getModulationIndex()   const { return modulationIndex; }

    // External (mirrored) mod sources: one slot for each *other* voice (up to 4)
    struct ExternalModParams
    {
        bool  enabled   = false;
        int   waveform  = 0;     // match your Waveform enum
        float freqHz    = 0.0f;  // absolute Hz (carrier_r * ratio_r)
        float index     = 0.0f;  // depth from source voice
        float amount01  = 0.0f;  // matrix slot 0..1 for r->this
    };

    void setExternalModSources(const ExternalModParams* srcs, int count);
    
    
    void setSelfModAmt(float sma) { selfModAmount = sma; };
    void setSequencerEngine(SequencerEngine* eng){ lane = eng;};
    void setModSeqEngine(SequencerEngine* eng){ modLane = eng;};
    
    void updateParamsPerBlock(VoiceParams vp);
    
private:
    bool modOctaveOn;
    bool modVibFreqOn;
    bool modVibDepthOn;
    
    float octave = 0.0f;       // octave shift in +/- multiples
    float vibFreq = 0.0f;      // vibrato frequency in Hz
    float vibDepth = 0.0f;     // vibrato depth in semitones
    float vibPhase = 0.0f;
    
    float sr;
    float osFactor;
    void updateParamsWithGlide(float value);
    VoiceParams voiceParams;
    SequencerEngine* lane = nullptr;
    SequencerEngine* modLane = nullptr;
    ExternalModParams extMod[4];
    float  extModPhase[4] { 0,0,0,0 };  // local phases for mirrored sources
    
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
    
    juce::ADSR quickReleaseEnv;
    juce::ADSR::Parameters quickReleaseEnvParams;
    bool isStealing = false;
    
    bool shouldAlias = false;
    juce::ADSR       env;
    juce::ADSR::Parameters envParams;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    
    Filter voiceFilter1, voiceFilter2;
    ResonatorProcessor resonator;
    

    float selfModAmount;
    
    float lastOut1, lastOut2;
    juce::AudioProcessorValueTreeState& apvts;
    float bpm, ppq;
};
