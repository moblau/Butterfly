#include "WahComponent.h"

WahComponent::WahComponent(juce::AudioProcessorValueTreeState& apvtsRef) : FXUnit(apvtsRef)
{
    setSize(200, 200);
    
    syncToggle.setButtonText("Sync Rate");
    addAndMakeVisible(syncToggle);
    // Attach toggle to a boolean parameter "WahUseSync" in the APVTS.
//    syncToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
//        apvts, "WahUseSync", syncToggle);
    syncToggleAttachment.reset( new ButtonAttachment(apvtsRef, "WahUseSync", syncToggle));
    // Whenever the toggle changes, show/hide the appropriate control:
    syncToggle.onClick = [this]() { updateSliderVisibility(); };
    
    syncRateBox.addItem("1/4",  1);
    syncRateBox.addItem("1/8",  2);
    syncRateBox.addItem("1/16", 3);
    syncRateBox.addItem("1/32", 4);
    // (IDs and strings should match whatever your processor expects)
    syncRateBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(syncRateBox);
//    syncRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
//        apvts, "WahSyncRate", syncRateBox);
    syncRateAttachment.reset( new ComboBoxAttachment(apvtsRef, "WahSyncRate", syncRateBox));
    updateSliderVisibility();
    
    freqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    freqSlider.setRange(1.0, 50.0, 0.1);
    freqSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(freqSlider);
    freqAttachment.reset (new Attachment (apvtsRef, "wahFreq", freqSlider));

    // Feedback: 0–1
    feedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    feedbackSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(feedbackSlider);
    feedbackAttachment.reset (new Attachment (apvtsRef, "wahFeedback", feedbackSlider));

    // Delay length: 5–50 samples
    delaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    delaySlider.setRange(5, 50, 1);
    delaySlider.setTextValueSuffix(" smp");
    addAndMakeVisible(delaySlider);
    delayAttachment.reset (new Attachment (apvtsRef, "wahDelay", delaySlider));
}

//void WahComponent::resized() {
//    freqSlider.setBounds(0, 20,66, 66);
//    feedbackSlider.setBounds(66, 20,66, 66);
//    delaySlider.setBounds(132, 20,66, 66);
//}
void WahComponent::resized()
{
    auto area = getLocalBounds();

    // Top: toggle button (fixed height)
    auto toggleArea = area.removeFromTop(100);
    syncToggle.setBounds(toggleArea.removeFromLeft(toggleArea.getWidth()/2));

    // Next row: either freqSlider or syncRateBox (same rectangle)
//    auto controlArea = area.removeFromTop(40);
    freqSlider.setBounds(toggleArea);
    syncRateBox.setBounds(toggleArea);

    // Below: feedback and delay knobs side by side
    auto knobsArea = area.removeFromTop(100);
    auto feedbackArea = knobsArea.removeFromLeft(knobsArea.getWidth() / 2);
    auto delayArea    = knobsArea;

    feedbackSlider.setBounds(feedbackArea);
    delaySlider.setBounds(delayArea);
}

void WahComponent::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
   g.setColour (juce::Colours::white);
   g.setFont (15.0f);
   g.drawFittedText ("Wah Controls", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
   
}

void WahComponent::updateSliderVisibility()
{
    const bool useSync = syncToggle.getToggleState();
    // When using sync, hide the frequency slider and show the dropdown:
    freqSlider.setVisible(!useSync);
    syncRateBox.setVisible(useSync);
    // Ensure labels/textboxes also hide along with the parent control
    freqSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    syncRateBox.setEnabled(useSync);
    repaint();
}
