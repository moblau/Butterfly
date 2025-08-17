#include "VoiceEditor.h"

VoiceEditor::VoiceEditor(juce::AudioProcessorValueTreeState& apvts, const juce::String& voicePrefix) :
carrierWaveform(apvts,juce::String("WAVEFORM"), voicePrefix),
modWaveform(apvts,juce::String("MOD_WAVEFORM"), voicePrefix),
panSlider("Pan", apvts, voicePrefix),
detuneSlider("Detune", apvts, voicePrefix),
modIndexSlider("Mod Amount", apvts, voicePrefix),
modRatioNumSlider("Num", apvts, voicePrefix),
modRatioDenSlider("Den", apvts, voicePrefix),
downsampleSlider("Downsample", apvts, voicePrefix),
freqSlider("Frequency", apvts, voicePrefix),
resSlider("Resonance", apvts, voicePrefix),
resonatorOffsetSlider("Resonator Offset", apvts, voicePrefix),
resonatorFeedbackSlider("Resonator Feedback", apvts, voicePrefix),
resonatorOffsetStrengthSlider("Offset Strength", apvts, voicePrefix)
{
    this->voicePrefix = voicePrefix;
//    setupSlider(panSlider,panSliderLabel,"Pan");
//    setupSlider(detuneSlider,detuneSliderLabel,"Detune");
//    setupSlider(modIndexSlider,modIndexSliderLabel,"Mod Amount");
//    setupSlider(modRatioNumSlider,modRatioNumSliderLabel,"Numerator");
//    setupSlider(modRatioDenSlider,modRatioDenSliderLabel,"Denominator");
//    setupSlider(downsampleSlider,downsampleSliderLabel,"Downsample");
//    setupSlider(freqSlider,freqSliderLabel,"Frequency");
//    setupSlider(resSlider,resSliderLabel,"Resonance");
//
    
    
    addAndMakeVisible(panSlider);
    addAndMakeVisible(detuneSlider);
    addAndMakeVisible(modIndexSlider);
    addAndMakeVisible(modRatioNumSlider);
    addAndMakeVisible(modRatioDenSlider);
    addAndMakeVisible(downsampleSlider);
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(resSlider);
    addAndMakeVisible(resonatorOffsetSlider);
    addAndMakeVisible(resonatorFeedbackSlider);
    addAndMakeVisible(resonatorOffsetStrengthSlider);
    
    //    setupComboBox(modWaveformBox);
//    setupComboBox(waveformBox);
    addAndMakeVisible(carrierWaveform);
    addAndMakeVisible(modWaveform);
    panAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "PAN"+voicePrefix, panSlider.getSlider());
    detuneAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "DETUNE"+voicePrefix, detuneSlider.getSlider());
    modIndexAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_INDEX"+voicePrefix, modIndexSlider.getSlider());
    modRatioNumAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_RATIO_NUM"+voicePrefix, modRatioNumSlider.getSlider());
    modRatioDenAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "MOD_RATIO_DEN"+voicePrefix, modRatioDenSlider.getSlider());
    downsampleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "DOWNSAMPLE"+voicePrefix, downsampleSlider.getSlider());
//    modWaveformAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,  "MOD_WAVEFORM"+voicePrefix, modWaveformBox);
//    waveformAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,  "WAVEFORM"+voicePrefix, waveformBox);
    
    freqAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "filter_freq"+voicePrefix, freqSlider.getSlider());
    resAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "filter_res"+voicePrefix, resSlider.getSlider());
    
    resOffsetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "res_offset"+voicePrefix, resonatorOffsetSlider.getSlider());
    resFeedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "res_feedback"+voicePrefix, resonatorFeedbackSlider.getSlider());
    resOffsetStrengthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,  "res_offset_strength"+voicePrefix, resonatorOffsetStrengthSlider.getSlider());


    
    aliasToggle.setButtonText("Alias");
    
    addAndMakeVisible(aliasToggle);

    aliasToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,  voicePrefix+"AliasToggle", aliasToggle);

    oscPreview = std::make_unique<WaveformVisualizer>(apvts, voicePrefix);
        addAndMakeVisible(*oscPreview);
    
}


void VoiceEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
//    label.attachToComponent(&slider, false); // false = place above the slider
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void VoiceEditor::setupComboBox(juce::ComboBox& box)
{
    box.addItem("Sine", 1);
    box.addItem("Saw", 2);
    box.addItem("Square", 3);
    box.setSelectedId(1);
    addAndMakeVisible(box);
}

//void VoiceEditor::resized()
//{
//    auto area = getLocalBounds().reduced(10);
//    auto rightArea = area.removeFromRight(getWidth()/2);
//    int padding = 5;
//    int controlHeight = 40;
//
//    freqSlider.setBounds(rightArea.removeFromTop(controlHeight));
//    rightArea.removeFromTop(padding);
//                         
//    resSlider.setBounds(rightArea.removeFromTop(controlHeight));
//    rightArea.removeFromTop(padding);
//    
//    waveformBox.setBounds(area.removeFromTop(24));
//    area.removeFromTop(padding);
//    
//    panSlider.setBounds(area.removeFromTop(controlHeight));
//    area.removeFromTop(padding);
//
//    detuneSlider.setBounds(area.removeFromTop(controlHeight));
//    area.removeFromTop(padding);
//
//    modIndexSlider.setBounds(area.removeFromTop(controlHeight));
//    area.removeFromTop(padding);
//
//    modWaveformBox.setBounds(area.removeFromTop(24));
//    area.removeFromTop(padding);
//
//    modRatioNumSlider.setBounds(area.removeFromTop(controlHeight));
//    area.removeFromTop(padding);
//
//    modRatioDenSlider.setBounds(area.removeFromTop(controlHeight));
//    downsampleSlider.setBounds(area.removeFromTop(controlHeight));
//    aliasToggle.setBounds(area.removeFromTop(controlHeight));
//}
void VoiceEditor::resized()
{
    auto area = getLocalBounds();
    auto left = area.removeFromLeft(getWidth()/2);
    auto topLeft = left.removeFromTop(getHeight()/2);

//    waveformBox.setBounds(topLeft.removeFromTop(topLeft.getHeight()/4));
    carrierWaveform.setBounds(topLeft.removeFromTop(topLeft.getHeight()/4));
    auto width = topLeft.getWidth()/4;
    
    panSlider.setBounds(topLeft.removeFromLeft(width).withHeight(110)
        .withY(topLeft.getY() + (topLeft.getHeight() - 110) / 2));

    detuneSlider.setBounds(topLeft.removeFromLeft(width).withHeight(110)
        .withY(topLeft.getY() + (topLeft.getHeight() - 110) / 2));

    downsampleSlider.setBounds(topLeft.removeFromLeft(width).withHeight(110)
        .withY(topLeft.getY() + (topLeft.getHeight() - 110) / 2));
    aliasToggle.setBounds(topLeft.removeFromLeft(width));
    
//    modWaveformBox.setBounds(left.removeFromTop(left.getHeight()/4));
    modWaveform.setBounds(left.removeFromTop(left.getHeight()/4));

    width = left.getWidth()/3;
    modIndexSlider.setBounds(left.removeFromLeft(width));
    modRatioNumSlider.setBounds(left.getX(),left.getY(),left.getWidth()/4,left.getHeight()/2);
    modRatioDenSlider.setBounds(left.getX(),left.getY()+ left.getHeight()/2,left.getWidth()/4,left.getHeight()/2);
    
    oscPreview->setBounds(left.getX()+left.getWidth()/4,left.getY(),left.getWidth()*3/4,left.getHeight());
    auto topRight = area.removeFromTop(area.getHeight()/2);
    freqSlider.setBounds(topRight.removeFromLeft(topRight.getWidth()/2).reduced(10));
    resSlider.setBounds(topRight.reduced(10));
    auto w = area.getWidth()/3;
    resonatorOffsetSlider.setBounds(area.removeFromLeft(w).withHeight(140)
        .withY(area.getY() + (area.getHeight() - (140)) / 2));

    resonatorFeedbackSlider.setBounds(area.removeFromLeft(w).withHeight((140))
        .withY(area.getY() + (area.getHeight() - (140)) / 2));

    resonatorOffsetStrengthSlider.setBounds(area.withHeight((140))
        .withY(area.getY() + (area.getHeight() - (140)) / 2));
    
//    placeLabelAboveSlider(panSliderLabel, panSlider,10,0);
//    placeLabelAboveSlider(detuneSliderLabel, detuneSlider,10,0);
//    placeLabelAboveSlider(downsampleSliderLabel, downsampleSlider,10,0);
//    placeLabelAboveSlider(modIndexSliderLabel, modIndexSlider,15,0);
//    placeLabelLeftOfSlider(modRatioNumSliderLabel, modRatioNumSlider, 20, 0);

    
}
void VoiceEditor::placeLabelLeftOfSlider(juce::Label& label, const juce::Slider& slider, int labelWidth, int spacing)
{
    auto sliderBounds = slider.getBounds();

    int labelX = sliderBounds.getX() - 20;
    int labelY = sliderBounds.getY()+5;
    
    label.setBounds(
        labelX,
        labelY,
        75,
        20
    );
}
void VoiceEditor::placeLabelAboveSlider(juce::Label& label, const juce::Slider& slider, int labelHeight = 16, int spacing = 4)
{
     auto sliderBounds = slider.getBounds();
     label.setBounds(
     sliderBounds.getX(),
     sliderBounds.getY() - labelHeight/3,
     sliderBounds.getWidth()+20,
     labelHeight
 );
}
                             
void updateSynthParameters(juce::AudioProcessorValueTreeState& apvts, float stepValue);

void VoiceEditor::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(0.5f); // inset for crisp lines

    g.setColour(juce::Colours::grey);

    // Draw the outer border
    g.drawRect(area, 1.0f);

    // Calculate midpoints
    float midX = area.getX() + area.getWidth()  * 0.5f;
    float midY = area.getY() + area.getHeight() * 0.5f;

    // Vertical center line
    g.drawLine(midX, area.getY(), midX, area.getBottom(), 1.0f);

    // Horizontal center line
    g.drawLine(area.getX(), midY, area.getRight(), midY, 1.0f);
}
