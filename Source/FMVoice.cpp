#include "FMVoice.h"

FMVoice::FMVoice(juce::AudioProcessorValueTreeState &apvtsRef,juce::AudioPlayHead* playHead, int voiceNum, int osFactor) : ds(0), apvts(apvtsRef), voiceFilter1(apvtsRef,playHead), voiceFilter2(apvtsRef,playHead), resonator(apvtsRef,playHead,voiceNum), osFactor(osFactor) {
    oversampling.reset(new juce::dsp::Oversampling<float>(2, osFactor, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true));
    quickReleaseEnvParams.attack = 0.0f;     // Instant attack
    quickReleaseEnvParams.decay = 0.0f;      // No decay
    quickReleaseEnvParams.sustain = 1.0f;    // Full sustain (until release)
    quickReleaseEnvParams.release = 0.1;  // Very short release, e.g., 5ms
    quickReleaseEnv.setParameters(quickReleaseEnvParams);
    voiceId = voiceNum;
    voiceFilter1.setVoiceNum(voiceId);
}

void FMVoice::prepare(double sampleRate, int samplesPerBlock){
    sr = sampleRate;
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
    voiceFilter1.prepare(sampleRate*osFactor, samplesPerBlock);
    voiceFilter1.setEnvelopeStatus(false);
    voiceFilter2.prepare(sampleRate*osFactor, samplesPerBlock);
    voiceFilter2.setEnvelopeStatus(true);
    
    resonator.prepare(sampleRate*osFactor,samplesPerBlock);
    lane->prepare(sampleRate*osFactor); //osFactor
    modLane->prepare(sampleRate*osFactor);

}

bool FMVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<FMSound*> (sound) != nullptr;
}

void FMVoice::startNote(int midiNoteNumber, float velocity,
                        juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    lastOut1 = 0;
    lastOut2 = 0;
    isStealing = false;
    env.reset();
    env.noteOn();
    voiceFilter2.startADSR();
    baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    
    float octaveShift = std::pow(2.0f, octave);
    carrierFrequency = baseFrequency * detuneRatio * octaveShift;
    modulationRatio  = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
    modulatorFrequency = carrierFrequency * modulationRatio;
    
    
//    carrierFrequency = baseFrequency * detuneRatio;
//    modulationRatio = static_cast<float>(modRatioNum) / static_cast<float>(modRatioDen);
//    modulatorFrequency = carrierFrequency * modulationRatio;
    
    
    this->midiNoteNumber = midiNoteNumber;
//    phase = 0.0f;
//    modPhase = 0.0f;
    
    phase = juce::Random::getSystemRandom().nextFloat() * juce::MathConstants<float>::twoPi;
    modPhase = juce::Random::getSystemRandom().nextFloat() * juce::MathConstants<float>::twoPi;
    
//    level = velocity / 127.0f * 10.0f;
    level = .1;
    
    pan = 0.5f;
    resonator.setDelayLines(baseFrequency);
//    delayLine.reset();
}

void FMVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        env.noteOff();  // trigger release phase
        voiceFilter2.stopADSR();
    }
    else
    {
        isStealing = true;
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
    

    // Main oversampled synthesis loop
    voiceFilter1.update();
    voiceFilter2.update();
    resonator.updateParameters();
    for (int i = 0; i < osNumSamples; ++i)
    {
        float stepValue = lane->valueAtSample(i, sr);
        float modStepValue = modLane->valueAtSample(i, sr);
//        updateParamsWithGlide(stepValue);
        
        float modNumNow = voiceParams.modNum;
        float modDenNow = voiceParams.modDen;
        if (voiceParams.modDenOn){
            modNumNow = juce::jlimit<float>(1,5,voiceParams.modNum+stepValue*5);
        }
        if (voiceParams.modNumOn){
            modDenNow = juce::jlimit<float>(1,5,voiceParams.modDen+stepValue*5);
        }
        setModulationRatio(modNumNow, modDenNow);
        
        if (voiceParams.modDetuneOn)
        {
            setDetune(juce::jlimit<float>(0,1,voiceParams.detune+stepValue));
        }
        
        
        // 1) modulator phase [0..1)
        float tMod = std::fmod(modPhase * inv2Pi, 1.0f);
        if (tMod < 0.0f) tMod += 1.0f;

        // 2) modulator output
        float rawMod = getSample(tMod, modWaveform, false, 0.0f);
        float modulationIndexNow = juce::jlimit<float>(1,50,modulationIndex+stepValue*50);
        if ( voiceParams.modAmountOn ){
            modSignal = rawMod * modulationIndexNow;
        }
        else{
            modSignal = rawMod*voiceParams.modIndex;
        }
        float panNow = pan;
        if (voiceParams.modPanOn){
            panNow = juce::jlimit<float>(0,1,panNow+stepValue);
        }
        float leftGain  = std::cos(panNow * juce::MathConstants<float>::pi * 0.5f);
        float rightGain = std::sin(panNow * juce::MathConstants<float>::pi * 0.5f);
        
        float sumExternal = 0.0f;
        for (int k = 0; k < 4; ++k)
        {
            const auto& M = extMod[k];
            if (!M.enabled || M.amount01 <= 0.0f || M.freqHz <= 0.0f) continue;

            // advance local phase at *oversampled* rate
            extModPhase[k] = std::fmod(extModPhase[k] + (M.freqHz * twoPi / osSampleRate), twoPi);
            float t = std::fmod(extModPhase[k] * inv2Pi, 1.0f); if (t < 0.0f) t += 1.0f;

            float raw = getSample(t, (Waveform) M.waveform, false, 0.0f);
            sumExternal += raw * M.index * M.amount01;
        }

        // add mirrored mod sources to your local FM signal
        float feedbackMod = std::tanh(lastOut1+lastOut2);
        modSignal += sumExternal +feedbackMod*selfModAmount;
        
        
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
        if (voiceParams.modDownsampleOn){
            ds.setFactor(juce::jlimit<float>(0,50,voiceParams.downsample+stepValue*50));
        }
        out = ds.processSample(out);

        // 9) write into oversampled buffer with pan & gain
        // Advance vibrato phase
        vibPhase = std::fmod(vibPhase + (twoPi * vibFreq / osSampleRate), twoPi);

        // Vibrato modulation in semitones
        
        // Advance vibrato phase
        float modVibFreq  = modVibFreqOn  ? vibFreq  * (1.0f + stepValue) : vibFreq;
        float modVibDepth = modVibDepthOn ? vibDepth * (1.0f + stepValue) : vibDepth;

        vibPhase = std::fmod(vibPhase + (twoPi * modVibFreq / osSampleRate), twoPi);

        // Vibrato modulation in semitones
        float vibSemitones = std::sin(vibPhase) * modVibDepth;

        // Convert semitones to frequency ratio
        float vibRatio = std::pow(2.0f, vibSemitones / 12.0f);

        // Compute octave shift (integer, snapped)
        int octaveShift = static_cast<int>(std::round(octave));

        if (modOctaveOn)
        {
            // Sequencer modulation: stepValue assumed -1..+1, scale up to ±5 semitone/octave offsets
            int stepOffset = static_cast<int>(std::round(stepValue * 5.0f));
            octaveShift += stepOffset;
        }

        // Clamp octave shift to ±2 octaves
        octaveShift = juce::jlimit(-2, 2, octaveShift);

        // Convert octave shift to frequency ratio
        float octaveRatio = std::pow(2.0f, octaveShift);

        // Final carrier and modulator frequencies
        float carrierNow   = baseFrequency * detuneRatio * octaveRatio * vibRatio;
        float modulatorNow = carrierNow * modulationRatio;

        // Advance phases at oversampled rate
        phase    = std::fmod(phase    + carrierNow   * twoPi / osSampleRate, twoPi);
        modPhase = std::fmod(modPhase + modulatorNow * twoPi / osSampleRate, twoPi);
//
//        // Convert semitones to frequency ratio
//        float vibRatio = std::pow(2.0f, vibSemitones / 12.0f);
//
//        // Recalculate carrier + modulator frequency with vibrato & octave
//        float octaveShift = std::pow(2.0f, octave);
//        float carrierNow = baseFrequency * detuneRatio * octaveShift * vibRatio;
//        float modulatorNow = carrierNow * modulationRatio;
//
//        // Use vibrato-modulated frequencies for phase increment
//        phase    = std::fmod(phase    + carrierNow   * twoPi / osSampleRate, twoPi);
//        modPhase = std::fmod(modPhase + modulatorNow * twoPi / osSampleRate, twoPi); 

        // 10) advance phases at oversampled rate
//        phase    = std::fmod(phase    + carrierFrequency    * twoPi / osSampleRate, twoPi);
//        modPhase = std::fmod(modPhase + modulatorFrequency * twoPi / osSampleRate, twoPi);

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
        lastOut1 = out;
        lastOut2 = lastOut1;
        
        //filterProcesing
        
        
//        voiceFilter1.setSeqStepValue(stepValue);
//        float filter1out = voiceFilter1.processSample(out, 0);
//        
//        voiceFilter2.setSeqStepValue(modStepValue);
//        float filter2out = voiceFilter2.processSample(filter1out, 0);
//        
//        resonator.setStepValue(stepValue);
//        float resOut = resonator.processSample(filter2out, 0);
        
        leftOS[i] = out * leftGain;
        if (rightOS)
            rightOS[i] = out * rightGain;
        
        
    }

    

    // Downsample back into the main buffer slice

    oversampling->processSamplesDown(tempBlock);
    
    float effectiveSampleRate = sr / std::pow(2.0f, osFactor);
    for (int i = 0; i < numSamples; ++i)
    {
        // Get lane values at normal sample rate
        float stepValue    = lane->valueAtSample(i, effectiveSampleRate);
        float modStepValue = modLane->valueAtSample(i, effectiveSampleRate);

        // Update filter and resonator parameters per sample
        voiceFilter1.setSeqStepValue(stepValue);
        float filter1Out = voiceFilter1.processSample(tempBuf.getSample(0, i), 0);

        voiceFilter2.setSeqStepValue(modStepValue);
        float filter2Out = voiceFilter2.processSample(filter1Out, 0);

        resonator.setStepValue(stepValue);
        float resOut = resonator.processSample(filter2Out, 0);

        // Write back to temp buffer
        tempBuf.setSample(0, i, resOut);
        if (tempBuf.getNumChannels() > 1)
            tempBuf.setSample(1, i, resOut); // simple mono copy to right channel
    }
    
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
    
//    voiceFilter1.update();
//    voiceFilter1.setSeqStepValue(stepValue);
//    voiceFilter1.process(tempBuf);
//    
//    voiceFilter2.update();
//    voiceFilter2.setSeqStepValue(stepValue);
//    voiceFilter2.process(tempBuf);
    
//    resonator.process(tempBuf);
    
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
    modulationIndex = juce::jlimit(0.0f, 100.0f, newIndex );
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


void FMVoice::setExternalModSources(const ExternalModParams* srcs, int count)
{
    for (int i = 0; i < juce::jmin(count, 4); ++i)
        extMod[i] = srcs[i];
}


void FMVoice::updateParamsPerBlock(VoiceParams vp)
{
    voiceParams = vp;
    setPan(vp.pan);
    setDetune(vp.detune);
    setAlias(vp.alias);
    setWaveform(vp.carrierWaveform);
    setModulatorWaveform(vp.modulatorWaveform);
    setModulationIndex(vp.modIndex);
    setModulationRatio(vp.modNum, vp.modDen);
    setSelfModAmt(vp.selfModAmt);
    setDownsampleFactor(vp.downsample);
    bpm = vp.bpm;
    ppq = vp.ppq;
    
    octave   = vp.octave;
    vibFreq  = vp.vibFreq;
    vibDepth = vp.vibDepth;
    modOctaveOn = vp.modOctaveOn;
    modVibFreqOn  = vp.modVibFreqOn;
    modVibDepthOn = vp.modVibDepthOn;
    
}

//void FMVoice::updateParamsWithGlide(float value)
//{
//    setPan(juce::jlimit(0.0f, 1.0f, pan + value));
//
//    setDetune(detuneRatio + value);
//    // detuneRatio is multiplicative, usually don’t clamp to [0,1] because it’s a ratio, not semitones.
//    // If you really want semitone offset clamped, do that before converting.
//
//    setModulationIndex(juce::jlimit(0.0f, 50.0f, modulationIndex + value * 50.0f));
//
//    setModulationRatio(
//        juce::jlimit(1, 5, modRatioNum + static_cast<int>(value * 5.0f)),
//        juce::jlimit(1, 5, modRatioDen + static_cast<int>(value * 5.0f))
//    );
//
//    setDownsampleFactor(juce::jlimit<float>(0, 50, dsFactor + value * 50.0f));
//}
