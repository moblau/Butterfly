/*
  ==============================================================================

    SequencerEngine.h
    Created: 7 Sep 2025 11:42:48pm
    Author:  Morris Blaustein

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct StepSequencerParams
{
    int currentStep;
    int offsets[8]= {0};
    float glideAmount;
    float stepStartTimes[8] = {0};
    float stepEndTimes[8] = {0};
};

class SequencerEngine
{
public:
    SequencerEngine();
    
    void updateParams(StepSequencerParams p) {
        params = p;
    }
    
    float updateBySample();
    
private:
    StepSequencerParams params;
    juce::AudioPlayHead* playHead;
};


