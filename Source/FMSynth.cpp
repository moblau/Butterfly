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
    for (int j = 0; j < getNumVoices(); ++j)
    {
        if (auto* voice = dynamic_cast<FMVoice*>(getVoice(j)))
        {
            juce::ADSR::Parameters envParams;
            envParams.attack  = *apvts.getRawParameterValue("attack" + idSuffix[0]) / 1000.0f;
            envParams.decay   = *apvts.getRawParameterValue("decay"  + idSuffix[0]) / 1000.0f;
            envParams.sustain = *apvts.getRawParameterValue("sustain" + idSuffix[0]);
            envParams.release = *apvts.getRawParameterValue("release" + idSuffix[0]) / 1000.0f;

            voice->setEnvelopeParams(envParams);
            int i = j;
            if ( i >= 4 ){
                i=i%4;
            } 
            int currentStep = static_cast<int>(*apvts.getRawParameterValue("seq" + idSuffix[i+1] + "CURRENT_STEP"));
            float stepValue = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "step" + juce::String(currentStep));
            
            bool modAmountActive = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "MOD_AMOUNT_ACTIVE");
            bool modNumActive = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "MOD_NUM_ACTIVE");
            bool modDenActive = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "MOD_DEN_ACTIVE");
            bool modGainActive = *apvts.getRawParameterValue("seq" + idSuffix[i+1] + "MOD_CARRIER_ACTIVE");
            bool shouldAlias = *apvts.getRawParameterValue(idSuffix[i+1]+"AliasToggle");
            float scaledStepRatio = stepValue * 10.0f - 5.0f;

            voice->setPan(*apvts.getRawParameterValue("PAN" + idSuffix[i+1]));

            int baseNumerator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_NUM" + idSuffix[i+1]));
            int modNumerator = baseNumerator + (modNumActive ? static_cast<int>(scaledStepRatio) : 0);
            modNumerator = juce::jlimit(1, 5, modNumerator);

            int baseDenominator = static_cast<int>(*apvts.getRawParameterValue("MOD_RATIO_DEN" + idSuffix[i+1]));
            int modDenominator = baseDenominator + (modDenActive ? static_cast<int>(scaledStepRatio) : 0);
            modDenominator = juce::jlimit(1, 5, modDenominator);

            voice->setModulationRatio(modNumerator, modDenominator);

            float baseIndex = *apvts.getRawParameterValue("MOD_INDEX" + idSuffix[i+1]);
            float scaledStepModAmt = stepValue * 100.0f - 50.0f;
            float modIndex = baseIndex + (modAmountActive ? scaledStepModAmt : 0.0f);
            modIndex = juce::jlimit(0.0f, 50.0f, modIndex);

            voice->setModulationIndex(modIndex);
            voice->setModulatorWaveform(static_cast<int>(*apvts.getRawParameterValue("MOD_WAVEFORM" + idSuffix[i+1])));
            voice->setDetune(*apvts.getRawParameterValue("DETUNE" + idSuffix[i+1]));
            voice->setWaveform(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM" + idSuffix[i+1])));
//            DBG(static_cast<int>(*apvts.getRawParameterValue("WAVEFORM" + idSuffix[i+1])));
            voice->setDownsampleFactor(static_cast<int>(*apvts.getRawParameterValue("DOWNSAMPLE" + idSuffix[i+1])));
            voice->setGain(stepValue);
            voice->setAlias(shouldAlias);
        }
    }
}

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
