#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ButterflyAudioProcessorEditor::ButterflyAudioProcessorEditor (ButterflyAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),
  stepSequencer(8, audioProcessor.apvts, "step"),
  voiceEditor1(p.apvts, "1"),
  voiceEditor2(p.apvts, "2"),
  voiceEditor3(p.apvts, "3"),
  voiceEditor4(p.apvts, "4")
{
    setSize (800, 800);

    auto& params = audioProcessor.apvts;

    // === Attachments ===
//    panAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "PAN1", panSlider1);
//    panAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "PAN2", panSlider2);
//    panAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "PAN3", panSlider3);
//    panAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "PAN4", panSlider4);
//
//    modIndexAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_INDEX1", modIndexSlider1);
//    modIndexAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_INDEX2", modIndexSlider2);
//    modIndexAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_INDEX3", modIndexSlider3);
//    modIndexAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_INDEX4", modIndexSlider4);
//
//    modWaveformAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "MOD_WAVEFORM1", modWaveformBox1);
//    modWaveformAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "MOD_WAVEFORM2", modWaveformBox2);
//    modWaveformAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "MOD_WAVEFORM3", modWaveformBox3);
//    modWaveformAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "MOD_WAVEFORM4", modWaveformBox4);
//
//    detuneAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "DETUNE1", detuneSlider1);
//    detuneAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "DETUNE2", detuneSlider2);
//    detuneAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "DETUNE3", detuneSlider3);
//    detuneAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "DETUNE4", detuneSlider4);
//
//    modRatioNumAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_NUM1", modRatioNumSlider1);
//    modRatioDenAttach1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_DEN1", modRatioDenSlider1);
//    modRatioNumAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_NUM2", modRatioNumSlider2);
//    modRatioDenAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_DEN2", modRatioDenSlider2);
//    modRatioNumAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_NUM3", modRatioNumSlider3);
//    modRatioDenAttach3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_DEN3", modRatioDenSlider3);
//    modRatioNumAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_NUM4", modRatioNumSlider4);
//    modRatioDenAttach4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "MOD_RATIO_DEN4", modRatioDenSlider4);
//
//    auto configureSlider = [](juce::Slider& s, bool isRotary = false) {
//        if (isRotary)
//            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
//        else
//            s.setSliderStyle(juce::Slider::LinearHorizontal);
//
//        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
//    };

//    auto rotaryRange = [](juce::Slider& s) { s.setRange(0.0, 50.0, 0.1); };
//
//    auto ratioRange = [](juce::Slider& s) { s.setRange(1.0, 5.0, 1.0); };
//
//    juce::Slider* allSliders[] = {
//        &panSlider1, &panSlider2, &panSlider3, &panSlider4,
//        &modIndexSlider1, &modIndexSlider2, &modIndexSlider3, &modIndexSlider4,
//        &detuneSlider1, &detuneSlider2, &detuneSlider3, &detuneSlider4,
//        &modRatioNumSlider1, &modRatioDenSlider1,
//        &modRatioNumSlider2, &modRatioDenSlider2,
//        &modRatioNumSlider3, &modRatioDenSlider3,
//        &modRatioNumSlider4, &modRatioDenSlider4
//    };

//    for (auto* s : allSliders)
//    {
//        configureSlider(*s);
//        addAndMakeVisible(*s);
//    }
//
//    rotaryRange(detuneSlider1); detuneSlider1.setRange(-0.5f, 0.5f, 0.01f);
//    rotaryRange(detuneSlider2); detuneSlider2.setRange(-0.5f, 0.5f, 0.01f);
//    rotaryRange(detuneSlider3); detuneSlider3.setRange(-0.5f, 0.5f, 0.01f);
//    rotaryRange(detuneSlider4); detuneSlider4.setRange(-0.5f, 0.5f, 0.01f);
//
//    ratioRange(modRatioNumSlider1); ratioRange(modRatioDenSlider1);
//    ratioRange(modRatioNumSlider2); ratioRange(modRatioDenSlider2);
//    ratioRange(modRatioNumSlider3); ratioRange(modRatioDenSlider3);
//    ratioRange(modRatioNumSlider4); ratioRange(modRatioDenSlider4);
//
//    auto setupWaveformBox = [](juce::ComboBox& box) {
//        box.addItem("Sine", 1);
//        box.addItem("Saw", 2);
//        box.addItem("Square", 3);
//        box.setSelectedId(1);
//    };
//
//    setupWaveformBox(modWaveformBox1);
//    setupWaveformBox(modWaveformBox2);
//    setupWaveformBox(modWaveformBox3);
//    setupWaveformBox(modWaveformBox4);
//
//    addAndMakeVisible(modWaveformBox1);
//    addAndMakeVisible(modWaveformBox2);
//    addAndMakeVisible(modWaveformBox3);
//    addAndMakeVisible(modWaveformBox4);
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
    int columnWidth = (getWidth() - margin * 2) / 4;
    int columnHeight = getHeight() - 200;
    // Reserve the bottom area for the step sequencer (e.g., 150 pixels high)
    auto totalArea = getLocalBounds();
    auto sequencerArea = totalArea.removeFromBottom(150);
    
    voiceEditor1.setBounds(margin + columnWidth * 0, margin, columnWidth, columnHeight);
    voiceEditor2.setBounds(margin + columnWidth * 1, margin, columnWidth, columnHeight);
    voiceEditor3.setBounds(margin + columnWidth * 2, margin, columnWidth, columnHeight);
    voiceEditor4.setBounds(margin + columnWidth * 3, margin, columnWidth, columnHeight);

    // Layout existing controls in the remaining area
//    int x1 = margin;
//    int x2 = x1 + columnWidth;
//    int x3 = x2 + columnWidth;
//    int x4 = x3 + columnWidth;
//
//    juce::Rectangle<int> bounds1(x1, margin, columnWidth, totalArea.getHeight());
//    juce::Rectangle<int> bounds2(x2, margin, columnWidth, totalArea.getHeight());
//    juce::Rectangle<int> bounds3(x3, margin, columnWidth, totalArea.getHeight());
//    juce::Rectangle<int> bounds4(x4, margin, columnWidth, totalArea.getHeight());
//
//    juce::Slider* pan[]   = { &panSlider1, &panSlider2, &panSlider3, &panSlider4 };
//    juce::Slider* detune[] = { &detuneSlider1, &detuneSlider2, &detuneSlider3, &detuneSlider4 };
//    juce::Slider* modIdx[] = { &modIndexSlider1, &modIndexSlider2, &modIndexSlider3, &modIndexSlider4 };
//    juce::ComboBox* waves[] = { &modWaveformBox1, &modWaveformBox2, &modWaveformBox3, &modWaveformBox4 };
//    juce::Slider* modNum[] = { &modRatioNumSlider1, &modRatioNumSlider2, &modRatioNumSlider3, &modRatioNumSlider4 };
//    juce::Slider* modDen[] = { &modRatioDenSlider1, &modRatioDenSlider2, &modRatioDenSlider3, &modRatioDenSlider4 };
//
//    for (int i = 0; i < 4; ++i)
//    {
//        int y = margin + i * (sliderHeight + padding);
//
//        pan[i]->setBounds(bounds1.getX(), y, columnWidth, sliderHeight);
//        detune[i]->setBounds(bounds2.getX(), y, rotarySize, rotarySize);
//        modIdx[i]->setBounds(bounds3.getX(), y, rotarySize, rotarySize);
//        waves[i]->setBounds(bounds3.getX(), y + rotarySize, rotarySize, comboBoxHeight);
//
//        modNum[i]->setBounds(bounds4.getX(), y, columnWidth, sliderHeight);
//        modDen[i]->setBounds(bounds4.getX(), y + sliderHeight/2 , columnWidth, sliderHeight);
//    }

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
