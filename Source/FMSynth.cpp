#include "FMSynth.h"
#include "FMVoice.h"  // if needed

FMSynth::FMSynth(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts){
    //use a look up table for ID's instead of creating string, better to keep the look up table
    //use a seperate file for ID's, make them static.
    for (int i = 0; i < idSuffix.size(); ++i)
            idSuffix[i] = juce::String(i);
    setNoteStealingEnabled(true);
}

void FMSynth::updateSynthParameters()
{
    // Helper to read float param quickly
    auto pfloat = [this](const juce::String& id) -> float
    {
        if (auto* p = apvts.getRawParameterValue(id)) return *p;
        jassertfalse; return 0.0f;
    };

    // Envelope 0 (matches your previous idSuffix[0] usage)
    {
        juce::ADSR::Parameters envParams;
        envParams.attack  = pfloat(PID::attack[0])  / 1000.0f;
        envParams.decay   = pfloat(PID::decay[0])   / 1000.0f;
        envParams.sustain = pfloat(PID::sustain[0]);
        envParams.release = pfloat(PID::release[0]) / 1000.0f;

        for (int j = 0; j < getNumVoices(); ++j)
            if (auto* v = dynamic_cast<FMVoice*>(getVoice(j)))
                v->setEnvelopeParams(envParams);
    }

    // Per-voice updates
    for (int j = 0; j < getNumVoices(); ++j)
    {
        auto* voice = static_cast<FMVoice*>(getVoice(j));
        if (!voice) continue;

        // Voices wrap to 0..3 for ID arrays
        int i = (j >= 4 ? j % 4 : j);
        
        VoiceParams vp;
        vp.detune = pfloat(PID::DETUNE[i]);
        vp.downsample = (int) pfloat(PID::DOWNSAMPLE[i]);
        vp.pan = pfloat(PID::PAN[i]);
        vp.modIndex = pfloat(PID::MOD_INDEX[i]);
        vp.modNum = (int) pfloat(PID::MOD_RATIO_NUM[i]);
        vp.modDen = (int) pfloat(PID::MOD_RATIO_DEN[i]);
        vp.carrierWaveform = pfloat(PID::CARR_WAVE[i]);
        vp.modulatorWaveform = pfloat(PID::MOD_WAVE[i]);
        vp.alias = pfloat(PID::aliasToggle[i]);
        vp.selfModAmt = pfloat(PID::MODMATRIX[i][i]);
        vp.bpm = bpm;
        vp.ppq = ppq;
        vp.modPanOn         = (pfloat(PID::PAN_mod[i])        != 0.0f);
        vp.modDetuneOn      = (pfloat(PID::DETUNE_mod[i])     != 0.0f);
        vp.modAmountOn      = (pfloat(PID::MODAMOUNT_mod[i])  != 0.0f);
        vp.modNumOn         = (pfloat(PID::NUM_mod[i])        != 0.0f);
        vp.modDenOn         = (pfloat(PID::DEN_mod[i])        != 0.0f);
        vp.modDownsampleOn  = (pfloat(PID::DOWNSAMPLE_mod[i]) != 0.0f);
        
        voice->updateParamsPerBlock(vp);
        
        
    }
}
