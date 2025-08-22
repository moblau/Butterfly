#include "FMSynth.h"
#include "FMVoice.h"  // if needed
#include "ParamIDs.h"

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

        // Choose sequencer bank by voice (1..4) -> we’ll map to 0..3; clamp to 0..4
        const int seqIdx = juce::jlimit(0, 4, i); // using seq1..seq4 (0..3); adjust if you want seq5 somewhere

        // CURRENT STEP and step value
        const int currentStep = (int) pfloat(PID::seqCurrentStep[seqIdx]);
        const float stepValue = pfloat(PID::seqStep[seqIdx][juce::jlimit(0, 7, currentStep)]);

        // Booleans (stored as float 0/1 in APVTS)
        const bool modAmountActive   = pfloat(PID::MODAMOUNT_mod[i])  != 0.0f;
        const bool modNumActive      = pfloat(PID::NUM_mod[i])        != 0.0f;
        const bool modDenActive      = pfloat(PID::DEN_mod[i])        != 0.0f;
        const bool modDetuneActive   = pfloat(PID::DETUNE_mod[i])     != 0.0f;
        const bool modPanActive      = pfloat(PID::PAN_mod[i])        != 0.0f;
        const bool modDownsampleAct  = pfloat(PID::DOWNSAMPLE_mod[i]) != 0.0f;
        const bool modResOffsetAct   = pfloat(PID::RESOFF_mod[i])     != 0.0f;
        const bool modResFeedbackAct = pfloat(PID::RESFDB_mod[i])     != 0.0f; // (not used below, but available)

        const bool shouldAlias       = pfloat(PID::aliasToggle[i])    != 0.0f;

        // Common scalings
        const float scaledStepRatio  = stepValue * 10.0f - 5.0f;   // for int-ish params (num/den)
        const float scaledStepModAmt = stepValue * 100.0f - 50.0f; // for mod index

        // Pan
        float panSeq = modPanActive ? stepValue : 0.0f;
        voice->setPan(pfloat(PID::PAN[i]) + panSeq);

        // Ratio Num/Den
        {
            int baseNum = (int) pfloat(PID::MOD_RATIO_NUM[i]);
            int baseDen = (int) pfloat(PID::MOD_RATIO_DEN[i]);

            int modNum = baseNum + (modNumActive ? (int)scaledStepRatio : 0);
            int modDen = baseDen + (modDenActive ? (int)scaledStepRatio : 0);

            modNum = juce::jlimit(1, 5, modNum);
            modDen = juce::jlimit(1, 5, modDen);

            voice->setModulationRatio(modNum, modDen);
        }

        // Mod index
        {
            float baseIndex = pfloat(PID::MOD_INDEX[i]);
            float modIndex  = baseIndex + (modAmountActive ? scaledStepModAmt : 0.0f);
            modIndex = juce::jlimit(0.0f, 50.0f, modIndex);
            voice->setModulationIndex(modIndex);
        }

        // Waves
        voice->setModulatorWaveform((int) pfloat(PID::MOD_WAVE[i]));
        voice->setWaveform((int) pfloat(PID::CARR_WAVE[i]));

        // Detune
        {
            float detuneSeq = modDetuneActive ? stepValue : 0.0f;
            voice->setDetune(pfloat(PID::DETUNE[i]) + detuneSeq);
        }

        // Downsample
        {
            float downsampleSeq = modDownsampleAct ? juce::jlimit(0.0f, 50.0f, stepValue * 50.0f) : 0.0f;
            int ds = (int) pfloat(PID::DOWNSAMPLE[i]) + (int) downsampleSeq;
            voice->setDownsampleFactor(ds);
        }

        // Misc per-voice settings
        voice->setGain(stepValue);
        voice->setAlias(shouldAlias);

        // (If you want to use modResOffsetAct/modResFeedbackAct here, wire them to your voice params)
    }
}

//void FMSynth::updateSynthParameters()
//{
//    for (int j = 0; j < getNumVoices(); ++j)
//    {
//        if (auto* voice = dynamic_cast<FMVoice*>(getVoice(j))) //static cast
//        {
//            juce::ADSR::Parameters envParams;
//            envParams.attack  = *apvts.getRawParameterValue("attack" + idSuffix[0]) / 1000.0f;
//            envParams.decay   = *apvts.getRawParameterValue("decay"  + idSuffix[0]) / 1000.0f;
//            envParams.sustain = *apvts.getRawParameterValue("sustain" + idSuffix[0]);
//            envParams.release = *apvts.getRawParameterValue("release" + idSuffix[0]) / 1000.0f;
//
//            voice->setEnvelopeParams(envParams);
//            int i = j;
//            if ( i >= 4 ){
//                i=i%4;
//            } 
//            int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + idSuffix[i+1] + "CURRENT_STEP"));
//            float stepValue = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "step" + juce::String(currentStep));
//            
//            bool modAmountActive = *apvts.getRawParameterValue("Mod Amount" + idSuffix[i+1] + "modulate");
//            
//            bool modNumActive = *apvts.getRawParameterValue("Num" + idSuffix[i+1] + "modulate");
//            bool modDenActive = *apvts.getRawParameterValue("Den" + idSuffix[i+1] + "modulate");
//            
//            bool modDetuneActive = *apvts.getRawParameterValue("Detune" + idSuffix[i+1] + "modulate");
//            bool modPanActive = *apvts.getRawParameterValue("Pan" + idSuffix[i+1] + "modulate");
//            bool modDownsampleActive = *apvts.getRawParameterValue("Downsample" + idSuffix[i+1] + "modulate");
//
//            bool modResOffsetActive = *apvts.getRawParameterValue("Resonator Offset" + idSuffix[i+1] + "modulate");
//            bool modResFeedbacksetActive = *apvts.getRawParameterValue("Resonator Feedback" + idSuffix[i+1] + "modulate");
//            
////            bool modGainActive = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "MOD_CARRIER_ACTIVE");
//            bool shouldAlias = *apvts.getRawParameterValue(idSuffix[i+1]+"AliasToggle");
//            float scaledStepRatio = stepValue * 10.0f - 5.0f;
//
//            float panSeq = 0;
//            if ( modPanActive ){
//                panSeq = stepValue;
//            }
//            voice->setPan(*apvts.getRawParameterValue("PAN" + idSuffix[i+1])+ panSeq);
//
//            int baseNumerator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM" + idSuffix[i+1]));
//            int modNumerator = baseNumerator + (modNumActive ? static_cast<int>(scaledStepRatio) : 0);
//            modNumerator = juce::jlimit(1, 5, modNumerator);
//
//            int baseDenominator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN" + idSuffix[i+1]));
//            int modDenominator = baseDenominator + (modDenActive ? static_cast<int>(scaledStepRatio) : 0);
//            modDenominator = juce::jlimit(1, 5, modDenominator);
//
//            voice->setModulationRatio(modNumerator, modDenominator);
//
//            float baseIndex = *apvts.getRawParameterValue("MOD_INDEX" + idSuffix[i+1]);
//            float scaledStepModAmt = stepValue * 100.0f - 50.0f;
//            float modIndex = baseIndex + (modAmountActive ? scaledStepModAmt : 0.0f);
//            modIndex = juce::jlimit(0.0f, 50.0f, modIndex);
//
//            voice->setModulationIndex(modIndex);
//            
//            voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM" + idSuffix[i+1])));
//            float detuneSeq = 0;
//            if ( modDetuneActive ){
//                detuneSeq = stepValue;
//            }
//            voice->setDetune(*apvts.getRawParameterValue("DETUNE" + idSuffix[i+1]) + detuneSeq);
//            
//            voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM" + idSuffix[i+1])));
////            DBG(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM" + idSuffix[i+1])));
//            float downsampleSeq = 0;
//            if ( modDownsampleActive ){
//                downsampleSeq = juce::jlimit(0.0f,50.0f,stepValue*50);
//            }
//            voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE" + idSuffix[i+1]))+static_cast<int>(downsampleSeq));
//            voice->setGain(stepValue);
//            voice->setAlias(shouldAlias);
//        }
//    }
//}

//void FMSynth::updateSynthParameters()
//{
//    for (int i = 0; i < getNumVoices(); ++i)
//    {
//        if (auto* voice = dynamic_cast<FMVoice*>(getVoice(i)))
//        {
//            switch (i)
//            {
//                case 0:
//                {
//                    int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + juce::String(i+1) + "CURRENT_STEP"));
//                
//                
//                    float stepValue = *apvts.getRawParameterValue("seq" + juce::String(i+1) +"step"+ juce::String(currentStep));  // Access the value of the current step
//                    bool modAmountActive = *apvts.getRawParameterValue("seq" + juce::String(i+1)+ "MOD_AMOUNT_ACTIVE");
//                    bool modNumActive    = *apvts.getRawParameterValue("seq" + juce::String(i+1)+"MOD_NUM_ACTIVE");
//                    bool modDenActive    = *apvts.getRawParameterValue("seq" + juce::String(i+1)+"MOD_DEN_ACTIVE");
//                    bool modGainActive    = *apvts.getRawParameterValue("seq" + juce::String(i+1)+ "MOD_CARRIER_ACTIVE");
//
//                    float scaledStepRatio = stepValue * 10.0f - 5.0f;
//
//                    voice->setPan(*apvts.getRawParameterValue("PAN1"));
//
//                    int baseNumerator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM1"));
//                    int modNumerator = baseNumerator + (modNumActive ? static_cast<int>(scaledStepRatio) : 0);
//                    modNumerator = juce::jlimit(1, 5, modNumerator);
//
//                    int baseDenominator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN1"));
//                    int modDenominator = baseDenominator + (modDenActive ? static_cast<int>(scaledStepRatio) : 0);
//                    modDenominator = juce::jlimit(1, 5, modDenominator);
//
//                    voice->setModulationRatio(modNumerator, modDenominator);
//
//                    float baseIndex = *apvts.getRawParameterValue("MOD_INDEX1");
//                    float scaledStepModAmt = stepValue * 100.0f - 50.0f;
//                    float modIndex = baseIndex + (modAmountActive ? scaledStepModAmt : 0.0f);
//                    modIndex = juce::jlimit(0.0f, 50.0f, modIndex);
//
//                    voice->setModulationIndex(modIndex);
//                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM1")));
//                    voice->setDetune(*apvts.getRawParameterValue("DETUNE1"));
//                    voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM1")));
//                    voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE1")));
//                    voice->setGain(stepValue);
//                    
//                    break;
//                    
//                }
//                case 1:
//                    voice->setPan(*apvts.getRawParameterValue("PAN2"));
//                    voice->setModulationRatio(
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM2")),
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN2"))
//                    );
//                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX2"));
//                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM2")));
//                    voice->setDetune(*apvts.getRawParameterValue("DETUNE2"));
//                    voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM2")));
//                    voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE2")));
//                    break;
//                case 2:
//                    voice->setPan(*apvts.getRawParameterValue("PAN3"));
//                    voice->setModulationRatio(
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM3")),
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN3"))
//                    );
//                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX3"));
//                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM3")));
//                    voice->setDetune(*apvts.getRawParameterValue("DETUNE3"));
//                    voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM3")));
//                    voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE3")));
//                    break;
//                case 3:
//                    voice->setPan(*apvts.getRawParameterValue("PAN4"));
//                    voice->setModulationRatio(
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM4")),
//                        static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN4"))
//                    );
//                    voice->setModulationIndex(*apvts.getRawParameterValue("MOD_INDEX4"));
//                    voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM4")));
//                    voice->setDetune(*apvts.getRawParameterValue("DETUNE4"));
//                    voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM4")));
//                    voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE4")));
//                    break;
//                default:
//                    break;
//            }
//        }
//    }
////    syncVoices();
//}

//void FMSynth::syncVoices(){
//    auto* voice1 = getVoicePointer(0);
//    auto* voice2 = getVoicePointer(1);
//
//    if ( voice2->getPhaseReset() ){
//        voice1->resetCarrierPhase();
//        voice2->resetModPhase();
//    }
//}
