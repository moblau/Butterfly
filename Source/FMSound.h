/*
  ==============================================================================

    FMSound.h
    Created: 19 Mar 2025 5:19:06pm
    Author:  Morris Blaustein

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FMSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};
