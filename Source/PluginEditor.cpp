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
  voiceTab1(audioProcessor.apvts,1),
  voiceTab2(audioProcessor.apvts,2),
  voiceTab3(audioProcessor.apvts,3),
  voiceTab4( audioProcessor.apvts,4),
  fxChainComponent(p.apvts),
  modTab(p.apvts)
{
    setSize (800, 600);
    setLookAndFeel(&customLookAndFeel);
    auto& params = audioProcessor.apvts;

    addAndMakeVisible(fxChainComponent);
//    addAndMakeVisible(voiceTab1);
//    addAndMakeVisible(voiceTab2);
//    addAndMakeVisible(voiceTab3);
//    addAndMakeVisible(voiceTab4);
//
    tabs.addTab("Voice 1",juce::Colours::transparentBlack, &voiceTab1, false);
    tabs.addTab("Voice 2",juce::Colours::transparentBlack, &voiceTab2, false);//    tabbedSeq.addTab ("Seq1", juce::Colours::transparentBlack, &stepSequencer1, false);

    tabs.addTab("Voice 3",juce::Colours::transparentBlack, &voiceTab3, false);//    tabbedSeq.addTab ("Seq1", juce::Colours::transparentBlack, &stepSequencer1, false);

    tabs.addTab("Voice 4",juce::Colours::transparentBlack, &voiceTab4, false);
    tabs.addTab("Modulation",juce::Colours::transparentBlack, &modTab, false);

//    tabbedSeq.addTab ("Seq1", juce::Colours::transparentBlack, &stepSequencer1, false);
//    tabbedSeq.addTab ("Seq2", juce::Colours::transparentBlack, &stepSequencer2, false);
//    tabbedSeq.addTab ("Seq3", juce::Colours::transparentBlack, &stepSequencer3, false);
//    tabbedSeq.addTab ("Seq4", juce::Colours::transparentBlack, &stepSequencer4, false);
    
   addAndMakeVisible(tabs);
//    addAndMakeVisible(stepSequencer);
    startTimer(10);
    
}

ButterflyAudioProcessorEditor::~ButterflyAudioProcessorEditor() {
    setLookAndFeel(nullptr);
    stopTimer();
}

void ButterflyAudioProcessorEditor::paint (juce::Graphics& g)
{

    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::slategrey);
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

//void ButterflyAudioProcessorEditor::resized()
//{
//    int margin = 20;
//    int padding = 20;
//    int sliderHeight = 100;
//    int rotarySize = 70;
//    int comboBoxHeight = 24;
//    int columnWidth = (getWidth() - margin * 2) / 5;
//    int columnHeight = getHeight() - 200;
//    // Reserve the bottom area for the step sequencer (e.g., 150 pixels high)
//    auto totalArea = getLocalBounds();
//    auto sequencerArea = totalArea.removeFromBottom(250);
//    auto fxChainArea = totalArea.removeFromRight(columnWidth);
//    fxChainComponent.setBounds(fxChainArea);
////    voiceEditor1.setBounds(margin + columnWidth * 0, margin, columnWidth, columnHeight);
////    voiceEditor2.setBounds(margin + columnWidth * 1, margin, columnWidth, columnHeight);
////    voiceEditor3.setBounds(margin + columnWidth * 2, margin, columnWidth, columnHeight);
////    voiceEditor4.setBounds(margin + columnWidth * 3, margin, columnWidth, columnHeight);
//    
//    voice.setBounds(sequencerArea.getX() + margin, sequencerArea.getY(), sequencerArea.getWidth() - margin * 2, sequencerArea.getHeight());
//    
//
//    // Layout the step sequencer at the bottom of the window
////    stepSequencer.setBounds(sequencerArea.getX() + margin, sequencerArea.getY(), sequencerArea.getWidth() - margin * 2, sequencerArea.getHeight());
//}
void ButterflyAudioProcessorEditor::resized()
{
    int margin = 20;
    int columnWidth = (getWidth() - margin * 2) / 5;
    auto area = getLocalBounds();

    // Right side: FX panel
    auto fxArea = area.removeFromRight(columnWidth);
    fxChainComponent.setBounds(fxArea);

    // Left/center: tabbed voices + sequencer
    auto mainArea = area.reduced(5);
    tabs.setBounds(mainArea);
}

//void ButterflyAudioProcessorEditor::timerCallback()
//{
//    if (auto* playhead = audioProcessor.getPlayHead())
//    {
//        juce::AudioPlayHead::CurrentPositionInfo info;
//        if (playhead->getCurrentPosition(info))
//        {
//            stepSequencer1.updateFromHostPositiƒon(info.ppqPosition, info.bpm);
//            stepSequencer2.updateFromHostPosition(info.ppqPosition, info.bpm);
//            stepSequencer3.updateFromHostPosition(info.ppqPosition, info.bpm);
//            stepSequencer4.updateFromHostPosition(info.ppqPosition, info.bpm);
//        }
//    }ƒ
//    stepSequencer1.repaint();
//}
void ButterflyAudioProcessorEditor::timerCallback()
{
    if (auto* playhead = audioProcessor.getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo info;
        if (playhead->getCurrentPosition(info))
        {
            voiceTab1.sequencer.updateFromHostPosition(info.ppqPosition, info.bpm);
            voiceTab2.sequencer.updateFromHostPosition(info.ppqPosition, info.bpm);
            voiceTab3.sequencer.updateFromHostPosition(info.ppqPosition, info.bpm);
            voiceTab4.sequencer.updateFromHostPosition(info.ppqPosition, info.bpm);

            // Optional: Only repaint the visible one if expensive
            if (auto* current = dynamic_cast<VoiceTabComponent*>(tabs.getCurrentContentComponent()))
            {
                current->sequencer.repaint();
            }
        }
    }
}
