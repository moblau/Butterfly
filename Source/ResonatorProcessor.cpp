

#include "ResonatorProcessor.h"

ResonatorProcessor::ResonatorProcessor(juce::AudioProcessorValueTreeState& apvtsRef,juce::AudioPlayHead* playHead) : DSPModule(apvtsRef,playHead) {}
ResonatorProcessor::~ResonatorProcessor() = default;

void ResonatorProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;                        // e.g. 44100.0
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = 2;

    for (auto& dl : delayLines)  {
        dl.prepare(spec);
        dl.setMaximumDelayInSamples(sr);
    }

    for (int i = 0; i < 4; i++){
        activeMidiNotes[i]=-1;
    }
    for (auto& filter : dampingFilters)
        filter.prepare(spec);
}

void ResonatorProcessor::reset(){
}

void ResonatorProcessor::setDelayLines(const juce::MidiBuffer& midiMessages)
{
    ratio = *apvts.getRawParameterValue("res_ratio");
    const float freq = *apvts.getRawParameterValue("res_filterfreq");
    for (auto& filter : dampingFilters)
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, freq);  // Dampen highs
    juce::MidiMessage message;
    int samplePosition = 0;
    // Create an iterator over the incoming MidiBuffer
    juce::MidiBuffer::Iterator it(midiMessages);
    // Loop until getNextEvent() returns false
    while (it.getNextEvent(message, samplePosition))
    {
        if (message.isNoteOn())
        {
            
            int noteNumber = message.getNoteNumber();
            // 1) Find the first free slot (activeMidiNotes[i] < 0)
            int freeSlot = -1;
            for (int i = 0; i < 4; ++i)
            {
                if (activeMidiNotes[i] < 0)
                {
                    freeSlot = i;
                    break;
                }
            }
            if (freeSlot >= 0)
            {
                
                activeMidiNotes[freeSlot] = noteNumber;
                
                // 2) Compute delay in samples for this MIDI note:
                //    JUCE helper to convert MIDI → frequency
                double freqHz = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
                double delaySeconds = 1.0 / freqHz;
                int delaySamples = static_cast<int>(std::round(delaySeconds * sr));
                
                // 3) Clamp to the maximum that this delay line allows
                delaySamples = juce::jlimit(1,
                                            delayLines[freeSlot].getMaximumDelayInSamples(),
                                            delaySamples);
                
                // 4) Set the delay length on that slot
                delayLines[freeSlot].setDelay(delaySamples*ratio);
            }
            // If no free slot, we ignore this extra NoteOn
        }
        else if (message.isNoteOff())
        {
            int noteNumber = message.getNoteNumber();
            
            // Find which slot had that note, and free it
            for (int i = 0; i < 4; ++i)
            {
                if (activeMidiNotes[i] == noteNumber)
                {
                    activeMidiNotes[i] = -1;

                    // Smoothly fade out the delay line content instead of abrupt reset
//                    const int fadeSamples = 64; // Short fade length
//                    float fade = 1.0f;
//
//                    for (int j = 0; j < fadeSamples; ++j)
//                    {
//                        fade = 1.0f - static_cast<float>(j) / static_cast<float>(fadeSamples);
//
//                        float currentL = delayLines[i].popSample(0);
//                        float currentR = delayLines[i].popSample(1);
//
//                        delayLines[i].pushSample(0, currentL * fade);
//                        delayLines[i].pushSample(1, currentR * fade);
//                    }
//
//                    // Optionally reset the delay line after fading
//                    delayLines[i].reset();  // Clears internal state (like filters), but no hard pop

                    break;
                }
            }
        }
    }
}
//void ResonatorProcessor::process(juce::AudioBuffer<float>& buffer)
//{
//    const int totalNumSamples  = buffer.getNumSamples();
//    const int totalNumChannels = buffer.getNumChannels();
//
//    const float feedbackAmount = *apvts.getRawParameterValue("res_feedback");
//    const float dryWetMix      = *apvts.getRawParameterValue("res_drywet");
//
//    for (int sample = 0; sample < totalNumSamples; ++sample)
//    {
//        float inputSample = 0.0f;
//
//        // Mix down all channels into one mono input
//        for (int ch = 0; ch < totalNumChannels; ++ch)
//            inputSample += buffer.getSample(ch, sample);
//
//        inputSample /= static_cast<float>(totalNumChannels); // normalize to mono
//
//        // --- Process each delay line independently ---
//        float wetSample = 0.0f;
//
//        for (int i = 0; i < 4; ++i)
//        {
//            if (activeMidiNotes[i] >= 0)
//            {
//                float delayOut = delayLines[i].popSample(0); // current delayed sample
//
//                // Feedback is local to this delay line
//                float feedbackSignal = inputSample + delayOut * feedbackAmount;
//
//                float dampedSignal = dampingFilters[i].processSample(feedbackSignal);
//                delayLines[i].pushSample(0, dampedSignal);
//
//                wetSample += delayOut; // accumulate total wet output
//            }
//        }
//
//        // --- Mix dry and wet output ---
//        float outputSample = dryWetMix * wetSample + (1.0f - dryWetMix) * inputSample;
//        outputSample = std::tanh(outputSample);   
//        for (int ch = 0; ch < totalNumChannels; ++ch)
//            buffer.setSample(ch, sample, outputSample);
//    }
//}

//
//void ResonatorProcessor::process(juce::AudioBuffer<float>& buffer)
//{
//    const int totalNumSamples  = buffer.getNumSamples();
//    const int totalNumChannels = buffer.getNumChannels();
//
//    const float feedbackAmount = *apvts.getRawParameterValue("res_feedback");
//    const float dryWetMix      = *apvts.getRawParameterValue("res_drywet");
//    
//
//    for (int sample = 0; sample < totalNumSamples; ++sample)
//    {
//        float inputSample = 0.0f;
//
//        // Mix down all channels into one mono input
//        for (int ch = 0; ch < totalNumChannels; ++ch)
//            inputSample += buffer.getSample(ch, sample);
//
//        inputSample /= static_cast<float>(totalNumChannels); // normalize to mono
//
//        // --- 1. Get sum of all delay line outputs (wet signal) ---
//        int activeCount = 0;
//        float wetSample = 0.0f;
//        for (int i = 0; i < 4; ++i)
//        {
//            if (activeMidiNotes[i] >= 0)
//            {
//                wetSample += delayLines[i].popSample(0);
//                ++activeCount;
//            }
//        }
//        if (activeCount > 0)
//            wetSample /= static_cast<float>(activeCount);
////        
//        // --- 2. Feed back signal into active delay lines ---
//        float feedbackSignal = inputSample + wetSample * feedbackAmount;
//        for (int i = 0; i < 4; ++i)
//        {
//            if (activeMidiNotes[i] >= 0)
//            {
//                float dampedSignal = dampingFilters[i].processSample(feedbackSignal);
//                delayLines[i].pushSample(0, dampedSignal);
//            }
//        }
//
//        // --- 3. Mix dry and wet signals ---
//        float outputSample = dryWetMix * wetSample + (1.0f - dryWetMix) * inputSample;
//
//        // --- 4. Write to all output channels ---
//        for (int ch = 0; ch < totalNumChannels; ++ch)
//            buffer.setSample(ch, sample, outputSample);
//    }
//}
// 
void ResonatorProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int totalNumSamples  = buffer.getNumSamples();
    const int totalNumChannels = buffer.getNumChannels();

    const float feedbackAmount = *apvts.getRawParameterValue("res_feedback");
    const float dryWetMix      = *apvts.getRawParameterValue("res_drywet");

    for (int sample = 0; sample < totalNumSamples; ++sample)
    {
        float inputL = buffer.getSample(0, sample);
        float inputR = (totalNumChannels > 1) ? buffer.getSample(1, sample) : inputL;

        float wetL = 0.0f;
        float wetR = 0.0f;

        for (int i = 0; i < 4; ++i)
        {
            if (activeMidiNotes[i] >= 0)
            {
                // Per-channel delay outputs
                float outL = delayLines[i].popSample(0);
                float outR = delayLines[i].popSample(1);

                // Per-channel feedback signals
                float fbL = inputL + outL * feedbackAmount;
                float fbR = inputR + outR * feedbackAmount;

                // Damping and push back
                float dampedL = dampingFilters[i].processSample(fbL);
                float dampedR = dampingFilters[i].processSample(fbR);

                delayLines[i].pushSample(0, dampedL);
                delayLines[i].pushSample(1, dampedR);

                // Accumulate wet outputs
                wetL += outL;
                wetR += outR;
            }
        }

        // Mix dry and wet per channel
        float outL = std::tanh(dryWetMix * wetL + (1.0f - dryWetMix) * inputL);
        float outR = std::tanh(dryWetMix * wetR + (1.0f - dryWetMix) * inputR);

        buffer.setSample(0, sample, outL);
        if (totalNumChannels > 1)
            buffer.setSample(1, sample, outR);
    }
}
