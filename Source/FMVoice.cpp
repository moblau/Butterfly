#include "FMVoice.h"

FMVoice::FMVoice() : ds(0) {
    oversampling.reset(new juce::dsp::Oversampling<float>(2, 4, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true));
    quickReleaseEnvParams.attack = 0.0f;     // Instant attack
    quickReleaseEnvParams.decay = 0.0f;      // No decay
    quickReleaseEnvParams.sustain = 1.0f;    // Full sustain (until release)
    quickReleaseEnvParams.release = 0.1;  // Very short release, e.g., 5ms
    quickReleaseEnv.setParameters(quickReleaseEnvParams);
}

void FMVoice::prepare(double sampleRate, int samplesPerBlock){
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;                        // e.g. 44100.0
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = 1;
    envParams.attack  = 0.01f;  // 10 ms
    envParams.release = 1;  // 20 ms
    envParams.sustain = 1.0;  // 20 ms
    envParams.decay = 1;  // 20 ms
    env.setParameters(envParams);
    env.setSampleRate(sampleRate);
    env.reset();
    oversampling->reset();
    oversampling->initProcessing(static_cast<size_t> (spec.maximumBlockSize));
    quickReleaseEnv.setSampleRate(sampleRate);
    quickReleaseEnv.reset();
}

bool FMVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<FMSound*> (sound) != nullptr;
}

void FMVoice::startNote(int midiNoteNumber, float velocity,
                        juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    isStealing = false;
    env.reset();
    env.noteOn();
    baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    carrierFrequency = baseFrequency * detuneRatio;
    modulationRatio = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
    modulatorFrequency = carrierFrequency * modulationRatio;
    this->midiNoteNumber = midiNoteNumber;
//    phase = 0.0f;
//    modPhase = 0.0f;
    
    phase = juce::Random::getSystemRandom().nextFloat() * juce::MathConstants<float>::twoPi;
    modPhase = juce::Random::getSystemRandom().nextFloat() * juce::MathConstants<float>::twoPi;
    
//    level = velocity / 127.0f * 10.0f;
    level = .1;
    
    pan = 0.5f;
    
//    delayLine.reset();
}

void FMVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        env.noteOff();  // trigger release phase
    }
    else
    {
        isStealing = true;
        DBG("stealing");
        quickReleaseEnv.reset();
        quickReleaseEnv.noteOn();
        quickReleaseEnv.noteOff();
    }
}

void FMVoice::pitchWheelMoved(int) {}
void FMVoice::controllerMoved(int, int) {}

void FMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                              int startSample,
                              int numSamples)
{
//    if (!isVoiceActive()) && !isStealing) // Only if not active AND not actively stealing
//           return;
    if (!isVoiceActive()) // Only if not active AND not actively stealing
           return;
    // Wrap the output buffer for DSP and carve out our slice
    juce::AudioBuffer<float> tempBuf(outputBuffer.getNumChannels(), numSamples);
    tempBuf.clear();

    juce::dsp::AudioBlock<float> tempBlock(tempBuf);
    
    // Upsample: get an oversampled block we can write into
    auto oversampledBlock = oversampling->processSamplesUp(tempBlock);

    // Compute oversampled sample‐rate and counts
    float osFactor      = (float) oversampling->getOversamplingFactor();
    float osSampleRate  = getSampleRate() * osFactor;
    int   osNumSamples  = (int) oversampledBlock.getNumSamples();

    // Prepare pointers and constants
    auto* leftOS  = oversampledBlock.getChannelPointer(0);
    auto* rightOS = oversampledBlock.getNumChannels() > 1
                    ? oversampledBlock.getChannelPointer(1) : nullptr;

    const float twoPi    = juce::MathConstants<float>::twoPi;
    const float inv2Pi   = 1.0f / twoPi;
    float leftGain  = std::cos(pan * juce::MathConstants<float>::pi * 0.5f);
    float rightGain = std::sin(pan * juce::MathConstants<float>::pi * 0.5f);

    // Main oversampled synthesis loop
    for (int i = 0; i < osNumSamples; ++i)
    {
        // 1) modulator phase [0..1)
        float tMod = std::fmod(modPhase * inv2Pi, 1.0f);
        if (tMod < 0.0f) tMod += 1.0f;

        // 2) modulator output
        float rawMod = getSample(tMod, modWaveform, false, 0.0f);
        modSignal = rawMod * modulationIndex;

        // 3) warped carrier phase
        float warped = std::fmod(phase + modSignal, twoPi);
        if (warped < 0.0f) warped += twoPi;

        // 4) normalized tCurr / tPrev
        float tCurr = std::fmod(warped * inv2Pi, 1.0f);
        if (tCurr < 0.0f) tCurr += 1.0f;
        float tPrev = std::fmod(prevWarpedPhase * inv2Pi, 1.0f);
        if (tPrev < 0.0f) tPrev += 1.0f;

        // 5) dtInst
        float dtInst = tCurr - tPrev;
        if (dtInst < 0.0f) dtInst += 1.0f;

        // 6) band-limited carrier
        float carrierSample = getSample(tCurr, waveform, true, dtInst);

        // 7) envelope + level
        float out = carrierSample * level;
        
        // 8) any other DSP (your ds)
        out = ds.processSample(out);

        // 9) write into oversampled buffer with pan & gain
        leftOS[i] = out * leftGain;
        if (rightOS)
            rightOS[i] = out * rightGain;

        // 10) advance phases at oversampled rate
        phase    = std::fmod(phase    + carrierFrequency    * twoPi / osSampleRate, twoPi);
        modPhase = std::fmod(modPhase + modulatorFrequency * twoPi / osSampleRate, twoPi);

        prevWarpedPhase = warped;
//        if (isStealing)
//            
//            {
////                quickReleaseEnv.noteOff();
//                quickReleaseEnv.applyEnvelopeToBuffer(tempBuf, 0, numSamples);
//                if (!quickReleaseEnv.isActive()) // If quick release is finished
//                {
//                    clearCurrentNote(); // Mark voice as free
//                    isStealing = false; // Reset flag
//                }
//            }
//            else if (!env.isActive()) // Normal release, check if main envelope is finished
//            {
//                clearCurrentNote(); // Mark voice as free
//            }
        if (!env.isActive()) // Normal release, check if main envelope is finished
        {
            clearCurrentNote(); // Mark voice as free
        }
    }



    // Downsample back into the main buffer slice

    oversampling->processSamplesDown(tempBlock);
//    env.applyEnvelopeToBuffer(tempBuf, 0, numSamples);
//    oversampling->processSamplesDown(tempBlock);

    if (isStealing)
    {
        quickReleaseEnv.applyEnvelopeToBuffer(tempBuf, 0, numSamples);
        if (!quickReleaseEnv.isActive())
        {
            clearCurrentNote(); // Voice is free again
            isStealing = false;
        }
    }
    else
    {
        env.applyEnvelopeToBuffer(tempBuf, 0, numSamples);
    }
    
    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
    {
        auto* dst = outputBuffer.getWritePointer(ch, startSample);
        auto* src = tempBuf.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
            dst[i] += src[i];  // accumulate voice output
    }


}

void FMVoice::resetModPhase() { phaseReset = false; }
bool FMVoice::getPhaseReset() { return phaseReset; }
void FMVoice::setPan(float newPan) { pan = juce::jlimit(0.0f, 1.0f, newPan); }

void FMVoice::setGain(float g){ gain = g;}
void FMVoice::setWaveform(int waveformId)
{
    switch (waveformId)
    {
        case 0: waveform = Waveform::Sine; break;
        case 1: waveform = Waveform::Saw; break;
        case 2: waveform = Waveform::Square; break;
        default: waveform = Waveform::Sine; break;
    }
}

void FMVoice::setModulatorWaveform(int waveformId)
{
    switch (waveformId)
    {
        case 0: modWaveform = Waveform::Sine; break;
        case 1: modWaveform = Waveform::Saw; break;
        case 2: modWaveform = Waveform::Square; break;
        default: modWaveform = Waveform::Sine; break;
    }
}

void FMVoice::setModulationRatio(int numerator, int denominator)
{
    modRatioNum = juce::jlimit(1, 16, numerator);
    modRatioDen = juce::jlimit(1, 16, denominator);
    modulationRatio = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
    modulatorFrequency = carrierFrequency * modulationRatio;
}

void FMVoice::setModulationIndex(float newIndex)
{
    modulationIndex = juce::jlimit(0.0f, 100.0f, newIndex);
}

void FMVoice::setDetune(float semitoneOffset)
{
    detuneRatio = std::pow(2.0f, semitoneOffset / 12.0f);
    carrierFrequency = baseFrequency * detuneRatio;
    modulatorFrequency = carrierFrequency * modulationRatio;
}

float FMVoice::getCarrierPhase() const { return phase; }
void FMVoice::resetCarrierPhase() { phase = 0.0f; }
float FMVoice::getModSignal() const { return modSignal; }

void FMVoice::setDownsampleFactor(int dsFactor)
{
    this->dsFactor = dsFactor;
    ds.setFactor(dsFactor);
}

float FMVoice::getSample(float t, Waveform wf, bool bandLimited, float dt)
{

    switch (wf)
    {
        case Waveform::Sine:
        {
            // t ∈ [0..1), convert to [0..2π)
            float angle = t * twoPi;
            return std::sin(angle);
        }

        case Waveform::Saw:
        {
            // naive saw from -1 to +1
            float sample = 2.0f * (t - 0.5f);

            if (bandLimited && shouldAlias)
            {
                sample -= polyBLEP(t, dt);
            }
            return sample;
        }

        case Waveform::Square:
        {
            float sample = (t < 0.5f) ? 1.0f : -1.0f;
            if (bandLimited && shouldAlias)
            {
                sample += polyBLEP(t, dt);
                float tFall = t + 0.5f;
                if (tFall >= 1.0f) tFall -= 1.0f;
                sample -= polyBLEP(tFall, dt);
            }
            return sample;
        }

        default:
            // fallback to sine
            return std::sin(t * twoPi);
    }
}

float FMVoice::polyBLEP(float t, float dt)
{
    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if (t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    else
        return 0.0f;
}

void FMVoice::setAlias(bool newAliasStaate){
    shouldAlias = newAliasStaate;
}
void FMVoice::setEnvelopeParams(const juce::ADSR::Parameters& newParams)
{
    envParams = newParams;
    env.setParameters(envParams);
}
