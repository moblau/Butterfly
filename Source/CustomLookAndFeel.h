
#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel();
    
//    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
//    {
//        juce::Slider::SliderLayout layout;
//
//        // Get the default bounds
//        layout.sliderBounds = slider.getLocalBounds();
//
//        // Reduce bounds to leave room for the text box
//        auto textBoxHeight = slider.getTextBoxHeight();
//        auto spacing = 30; // ← adjust this to raise/lower the textbox
//        layout.textBoxBounds = juce::Rectangle<int>(
//            slider.getLocalBounds().getX(),
//            slider.getLocalBounds().getBottom() - textBoxHeight - spacing,
//            slider.getWidth(),
//            textBoxHeight
//        );
//
//        // Shrink the slider bounds vertically to avoid overlapping the textbox
//        layout.sliderBounds.removeFromBottom(textBoxHeight + spacing);
//
//        return layout;
//    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;
//    
//    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    // length of the little arrow
    
    void drawBubble(juce::Graphics& g,
                        juce::BubbleComponent& bubble/*bubble*/,
                        const juce::Point<float>& tip,
                        const juce::Rectangle<float>& body) override;
    
    void setModulationStatus(bool shouldModulate){ isModulated = shouldModulate; };
    
    void setModAmount(int amt) { modAmount = amt; };
    
private:
    int isModulated;
    float modAmount;
};


class StepSequencerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    StepSequencerLookAndFeel();
    ~StepSequencerLookAndFeel();
    
//    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
//    {
//        juce::Slider::SliderLayout layout;
//
//        // Get the default bounds
//        layout.sliderBounds = slider.getLocalBounds();
//
//        // Reduce bounds to leave room for the text box
//        auto textBoxHeight = slider.getTextBoxHeight();
//        auto spacing = 30; // ← adjust this to raise/lower the textbox
//        layout.textBoxBounds = juce::Rectangle<int>(
//            slider.getLocalBounds().getX(),
//            slider.getLocalBounds().getBottom() - textBoxHeight - spacing,
//            slider.getWidth(),
//            textBoxHeight
//        );
//
//        // Shrink the slider bounds vertically to avoid overlapping the textbox
//        layout.sliderBounds.removeFromBottom(textBoxHeight + spacing);
//
//        return layout;
//    }
    
//    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
//                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
//    
//    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
//                              float sliderPos, float minSliderPos, float maxSliderPos,
//                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;
////
////    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
//    
//    void setModulationStatus(bool shouldModulate){ isModulated = shouldModulate; };
//    

    
private:
    bool isModulated;
};
