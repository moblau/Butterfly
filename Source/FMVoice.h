
/*
  ==============================================================================

    FMVoice.h
    Created: 19 Mar 2025 5:19:14pm
    Author:  Morris Blaustein

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FMSound.h"

class FMVoice : public juce::SynthesiserVoice
{
public:
    FMVoice() {}

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<FMSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int /*currentPitchWheelPosition*/) override
    {
        baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        carrierFrequency = baseFrequency * detuneRatio;
        modulationRatio = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
        modulatorFrequency = carrierFrequency * modulationRatio;

        phase = 0.0f;
        modPhase = 0.0f;
        level = velocity/127.0*10;

        pan = 0.5f; // Default center
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
            level = 0.0f;
        else
            clearCurrentNote();
    }

    void pitchWheelMoved(int newPitchWheelValue) override {};
    void controllerMoved(int controllerNumber, int newControllerValue) override {};

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (carrierFrequency == 0.0f)
            return;

        float leftGain = std::cos(pan * juce::MathConstants<float>::pi * 0.5f);
        float rightGain = std::sin(pan * juce::MathConstants<float>::pi * 0.5f);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Generate modulator signal
            float modSignal = getModulatorSample(modPhase) * modulationIndex;
            
            float output = std::sin(phase + modSignal) * level;

            // Apply panning
            float outputL = output * leftGain;
            float outputR = output * rightGain;

            // Write to buffer
            outputBuffer.addSample(0, startSample + sample, outputL); // Left
            outputBuffer.addSample(1, startSample + sample, outputR); // Right

            // Increment phases
            phase += carrierFrequency * twoPi / getSampleRate();
            modPhase += modulatorFrequency * twoPi / getSampleRate();

            if (phase > twoPi) phase -= twoPi;
            if (modPhase > twoPi) modPhase -= twoPi;
        }
    }

    void setPan(float newPan) { pan = juce::jlimit(0.0f, 1.0f, newPan); }
    void setModulatorWaveform(int waveformId)
    {
        switch (waveformId)
        {
            case 0: modWaveform = ModulatorWaveform::Sine; break;
            case 1: modWaveform = ModulatorWaveform::Saw; break;
            case 2: modWaveform = ModulatorWaveform::Square; break;
            default: modWaveform = ModulatorWaveform::Sine; break;
        }
    }

    void setModulationRatio(int numerator, int denominator) {
        modRatioNum = juce::jlimit(1, 16, numerator);
        modRatioDen = juce::jlimit(1, 16, denominator);
        modulationRatio = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
        modulatorFrequency = carrierFrequency * modulationRatio;
    }

    void setModulationIndex(float newIndex) {
        modulationIndex = juce::jlimit(0.0f, 100.0f, newIndex);
    }

    void setDetune(float semitoneOffset)
    {
        detuneRatio = std::pow(2.0f, semitoneOffset / 12.0f);
        carrierFrequency = baseFrequency * detuneRatio;
        modulatorFrequency = carrierFrequency * modulationRatio;
    }
   
private:
    enum class ModulatorWaveform
    {
        Sine,
        Saw,
        Square
    };

    float getModulatorSample(float phase)
    {
        switch (modWaveform)
        {
            case ModulatorWaveform::Sine:
                return std::sin(phase);
            case ModulatorWaveform::Saw:
                return 2.0f * (phase / twoPi - std::floor(phase / twoPi + 0.5f)); // Sawtooth (-1 to 1)
            case ModulatorWaveform::Square:
                return (std::fmod(phase, twoPi) < juce::MathConstants<float>::pi ? 1.0f : -1.0f);
            default:
                return std::sin(phase);
        }
    }

    ModulatorWaveform modWaveform = ModulatorWaveform::Sine;
    float phase = 0.0f, modPhase = 0.0f;
    float carrierFrequency = 0.0f, modulatorFrequency = 0.0f;
    float level = 0.0f;
    float baseFrequency = 0.0f;
    float detuneRatio = 1.0f;
    float modulationIndex = 2.0f;
    float modulationRatio = 3.0f; // Modulator-to-carrier ratio
    int modRatioNum = 3;
    int modRatioDen = 1;

    float pan = 0.5f; // 0 = left, 1 = right

    const float twoPi = juce::MathConstants<float>::twoPi;
};
