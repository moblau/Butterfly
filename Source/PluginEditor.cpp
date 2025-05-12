#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ButterflyAudioProcessorEditor::ButterflyAudioProcessorEditor (ButterflyAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),
  stepSequencer(8, audioProcessor.apvts, "step"),
  voiceEditor1(p.apvts, "1"),
  voiceEditor2(p.apvts, "2"),
  voiceEditor3(p.apvts, "3"),
  voiceEditor4(p.apvts, "4"), fxChainComponent(p.apvts)
{
    setSize (1500, 1000);

    auto& params = audioProcessor.apvts;

    addAndMakeVisible(fxChainComponent);
    addAndMakeVisible(voiceEditor1);
    addAndMakeVisible(voiceEditor2);
    addAndMakeVisible(voiceEditor3);
    addAndMakeVisible(voiceEditor4);
    addAndMakeVisible(stepSequencer);
    startTimer(10);
    
}

ButterflyAudioProcessorEditor::~ButterflyAudioProcessorEditor() {}

void ButterflyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void ButterflyAudioProcessorEditor::resized()
{
    int margin = 20;
    int padding = 20;
    int sliderHeight = 100;
    int rotarySize = 70;
    int comboBoxHeight = 24;
    int columnWidth = (getWidth() - margin * 2) / 5;
    int columnHeight = getHeight() - 200;
    // Reserve the bottom area for the step sequencer (e.g., 150 pixels high)
    auto totalArea = getLocalBounds();
    auto sequencerArea = totalArea.removeFromBottom(150);
    auto fxChainArea = totalArea.removeFromRight(columnWidth);
    fxChainComponent.setBounds(fxChainArea);
    voiceEditor1.setBounds(margin + columnWidth * 0, margin, columnWidth, columnHeight);
    voiceEditor2.setBounds(margin + columnWidth * 1, margin, columnWidth, columnHeight);
    voiceEditor3.setBounds(margin + columnWidth * 2, margin, columnWidth, columnHeight);
    voiceEditor4.setBounds(margin + columnWidth * 3, margin, columnWidth, columnHeight);

    

    // Layout the step sequencer at the bottom of the window
    stepSequencer.setBounds(sequencerArea.getX() + margin, sequencerArea.getY(), sequencerArea.getWidth() - margin * 2, sequencerArea.getHeight());
}

void ButterflyAudioProcessorEditor::timerCallback()
{
    int step = audioProcessor.currentStepAtom.load();
    // Pass the value to the StepSequencer
    stepSequencer.setCurrentStep(step);
    // Optionally repaint if needed.
    stepSequencer.repaint();
}
