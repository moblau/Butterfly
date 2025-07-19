#include "VoiceEditor.h"

VoiceEditor::VoiceEditor(juce::AudioProcessorValueTreeState& apvts, const juce::String& voicePrefix) :
carrierWaveform(apvts,juce::String("WAVEFORM"), voicePrefix),
modWaveform(apvts,juce::String("MOD_WAVEFORM"), voicePrefix)
{
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


    
    aliasToggle.setButtonText("Alias");
    
    addAndMakeVisible(aliasToggle);

    aliasToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,  voicePrefix+"AliasToggle", aliasToggle);

    
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
    
    panSlider.setBounds(topLeft.removeFromLeft(width));
    detuneSlider.setBounds(topLeft.removeFromLeft(width));
    downsampleSlider.setBounds(topLeft.removeFromLeft(width));
    aliasToggle.setBounds(topLeft.removeFromLeft(width));
    
//    modWaveformBox.setBounds(left.removeFromTop(left.getHeight()/4));
    modWaveform.setBounds(left.removeFromTop(left.getHeight()/4));

    width = left.getWidth()/2;
    modIndexSlider.setBounds(left.removeFromLeft(width));
    modRatioNumSlider.setBounds(left.removeFromTop(left.getHeight()/2));
    modRatioDenSlider.setBounds(left);
    
    auto topRight = area.removeFromTop(area.getHeight()/2);
    freqSlider.setBounds(topRight.removeFromLeft(topRight.getWidth()/2));
    resSlider.setBounds(topRight);
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
