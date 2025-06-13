#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//ButterflyAudioProcessorEditor::ButterflyAudioProcessorEditor (ButterflyAudioProcessor& p)
//: AudioProcessorEditor (&p), audioProcessor (p),
//  stepSequencer1(8, 1,audioProcessor.apvts),
//  stepSequencer2(8, 2,audioProcessor.apvts),
//  stepSequencer3(8, 3,audioProcessor.apvts),
//  stepSequencer4(8, 4, audioProcessor.apvts),
//  voiceEditor1(p.apvts, "1"),
//  voiceEditor2(p.apvts, "2"),
//  voiceEditor3(p.apvts, "3"),
//  voiceEditor4(p.apvts, "4"), fxChainComponent(p.apvts)
//{
ButterflyAudioProcessorEditor::ButterflyAudioProcessorEditor (ButterflyAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),
  stepSequencer1(8, 1,audioProcessor.apvts),
  stepSequencer2(8, 2,audioProcessor.apvts),
  stepSequencer3(8, 3,audioProcessor.apvts),
  stepSequencer4(8, 4, audioProcessor.apvts),
  voiceEditor1(p.apvts, "1"),
  voiceEditor2(p.apvts, "2"),
  voiceEditor3(p.apvts, "3"),
  voiceEditor4(p.apvts, "4"), fxChainComponent(p.apvts)
{
    setSize (1000, 800);

    auto& params = audioProcessor.apvts;

    addAndMakeVisible(fxChainComponent);
    addAndMakeVisible(voiceEditor1);
    addAndMakeVisible(voiceEditor2);
    addAndMakeVisible(voiceEditor3);
    addAndMakeVisible(voiceEditor4);
    
    tabbedSeq.addTab ("Seq1", juce::Colours::transparentBlack, &stepSequencer1, false);
    tabbedSeq.addTab ("Seq1", juce::Colours::transparentBlack, &stepSequencer1, false);
    tabbedSeq.addTab ("Seq2", juce::Colours::transparentBlack, &stepSequencer2, false);
    tabbedSeq.addTab ("Seq3", juce::Colours::transparentBlack, &stepSequencer3, false);
    tabbedSeq.addTab ("Seq4", juce::Colours::transparentBlack, &stepSequencer4, false);
    
    addAndMakeVisible(tabbedSeq);
//    addAndMakeVisible(stepSequencer);
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
    auto sequencerArea = totalArea.removeFromBottom(250);
    auto fxChainArea = totalArea.removeFromRight(columnWidth);
    fxChainComponent.setBounds(fxChainArea);
    voiceEditor1.setBounds(margin + columnWidth * 0, margin, columnWidth, columnHeight);
    voiceEditor2.setBounds(margin + columnWidth * 1, margin, columnWidth, columnHeight);
    voiceEditor3.setBounds(margin + columnWidth * 2, margin, columnWidth, columnHeight);
    voiceEditor4.setBounds(margin + columnWidth * 3, margin, columnWidth, columnHeight);
    
    tabbedSeq.setBounds(sequencerArea.getX() + margin, sequencerArea.getY(), sequencerArea.getWidth() - margin * 2, sequencerArea.getHeight());
    

    // Layout the step sequencer at the bottom of the window
//    stepSequencer.setBounds(sequencerArea.getX() + margin, sequencerArea.getY(), sequencerArea.getWidth() - margin * 2, sequencerArea.getHeight());
}

void ButterflyAudioProcessorEditor::timerCallback()
{
    if (auto* playhead = audioProcessor.getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo info;
        if (playhead->getCurrentPosition(info))
        {
            stepSequencer1.updateFromHostPosition(info.ppqPosition, info.bpm);
            stepSequencer2.updateFromHostPosition(info.ppqPosition, info.bpm);
            stepSequencer3.updateFromHostPosition(info.ppqPosition, info.bpm);
            stepSequencer4.updateFromHostPosition(info.ppqPosition, info.bpm);
        }
    }
    stepSequencer1.repaint();
}
