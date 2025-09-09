/*
  ==============================================================================

    ComponentUtility.h
    Created: 18 Jul 2025 5:43:46am
    Author:  Morris Blaustein

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "ParamIDs.h"

class SliderWithLabel : public juce::Component, private juce::Timer
{
public:
    SliderWithLabel() = default;
    
    SliderWithLabel(juce::String newLabel, juce::AudioProcessorValueTreeState& apvtsRef, juce::String prefix) : apvts(apvtsRef), prefix(prefix){
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
//        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        
        slider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(sliderValueLabel);
        hideValueBubble();
        slider.onDragStart   = [this]{ showOrUpdateValueBubble(); };
        slider.onValueChange = [this]{ showOrUpdateValueBubble(); };
        slider.onDragEnd     = [this]{ hideValueBubble(); };
        
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setPopupDisplayEnabled(false, false, &slider);
        
        addAndMakeVisible(slider);
        // Listen for right-clicks on the slider to toggle modulation colour
        slider.addMouseListener(this, false);

        label.setText(newLabel, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        paramName = newLabel;
        if ( auto * param = apvtsRef.getParameter(newLabel+prefix+"modulate")){
            isModulated = param->getValue();
            customLookAndFeel.setModulationStatus(isModulated);
            canBeModulated = true;
        }
        else{
            isModulated = false;
            canBeModulated = false;
        }
        startTimerHz(30);
        
    }
    ~SliderWithLabel() override
    {
        slider.removeMouseListener(this);
        slider.setLookAndFeel(nullptr); // IMPORTANT: prevent dangling pointer
    }
    juce::Slider& getSlider() { return slider; }
    juce::Label& getLabel() { return label; }
    // member in your parent component
    
    void showOrUpdateValueBubble()
    {
        sliderValueLabel.setVisible(true);
        sliderValueLabel.setText(juce::String(slider.getValue()), juce::NotificationType::dontSendNotification);

    }

    void hideValueBubble()
    {
        sliderValueLabel.setVisible(false);
        
    }
    
    // Allow external control of the visual modulated state
    void setModulated(bool shouldBeBlue)
    {
        isModulated = shouldBeBlue;
        if (isModulated)
        {
            slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue);
            slider.setColour(juce::Slider::thumbColourId, juce::Colours::deepskyblue);
        }
        else
        {
            // Reset to default look-and-feel colours by clearing overrides
            slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour());
            slider.setColour(juce::Slider::thumbColourId, juce::Colour());
        }
        repaint();
    }
    bool getIsModulated() const { return isModulated; }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        label.setBounds(bounds.removeFromTop(20));
        slider.setBounds(bounds);
        int labelWidth, labelHeight;
        if (slider.getWidth() > 50){
            labelWidth = 43;
            labelHeight = 20;
        }
        else{
            labelWidth = 20;
            labelHeight = 20;
        }

        sliderValueLabel.setBounds(bounds.withSizeKeepingCentre(labelWidth, labelHeight));
        sliderValueLabel.setVisible(false);

    }

    // Toggle modulation colour on right-click (including ctrl-click on macOS)
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (canBeModulated && (e.mods.isRightButtonDown() || e.mods.isCtrlDown()) ){
            if ( isModulated == 0 ){
                isModulated = 1;
            }
            else{
                isModulated = 0;
            }
            if ( auto * param = apvts.getParameter(paramName+prefix+"modulate")){
                param->setValueNotifyingHost(isModulated);
                customLookAndFeel.setModulationStatus(isModulated);
            }
        }
        
    }
private:
    float pfloat(const juce::String& id) const
    {
        if (auto* p = apvts.getRawParameterValue(id))
            return p->load();
        jassertfalse; return 0.f;
    }
    
    void timerCallback() override
    {
        // 1) Keep visual "modulated" state in sync with the parameter (if present)
        if (isModulated){
            // 2) Compute seqIndex from your prefix (e.g. "0", "1", "2", ...)
            const int seqIndex = prefix.getIntValue()-1 ; // robust even if prefix has non-digits at end

            // 3) Pull current step + step value
            // NOTE: adjust PID::... IDs to match your exact parameter IDs
            const int currentStep = (int) pfloat(PID::seqCurrentStep[seqIndex]);

            // Clamp the step index (0..7 here; adapt to your step count)
            const int clampedStep = juce::jlimit(0, 7, currentStep);
            const float stepValue = pfloat(PID::seqStep[seqIndex][clampedStep]); // expect 0..1
            // 4) Push the mod amount into the L&F so it can draw a ring/arc
            customLookAndFeel.setModAmount(stepValue);

            // 5) Repaint the slider only (cheap)
            slider.repaint();
        }

        
    }

    
    
    juce::Label sliderValueLabel;

    juce::Slider slider;
    juce::Label label;
    int isModulated = false;
    bool canBeModulated;
    
    CustomLookAndFeel customLookAndFeel;
    juce::AudioProcessorValueTreeState& apvts;
    juce::String paramName;
    juce::String prefix;
    int seqIndex;
};

class WaveformSelector : public juce::Component,
                         private juce::AudioProcessorValueTreeState::Listener
{
public:
    WaveformSelector(juce::AudioProcessorValueTreeState& apvts,
                     juce::String waveformType,
                     juce::String waveformIndex)
        : apvtsRef(apvts)
    {
        // Images + buttons (unchanged)
        sineOn  = juce::ImageFileFormat::loadFrom(BinaryData::SineOn_png,   BinaryData::SineOn_pngSize);
        sineOff = juce::ImageFileFormat::loadFrom(BinaryData::SineOff_png,  BinaryData::SineOff_pngSize);
        sineButton.setImages(false, true, true, sineOff, 1.0f, {}, sineOn, 1.0f, {}, sineOn, 1.0f, {});
        addAndMakeVisible(sineButton);
        sineButton.onClick = [this]() { userSelect(0); };

        sawOn   = juce::ImageFileFormat::loadFrom(BinaryData::SawOn_png,    BinaryData::SawOn_pngSize);
        sawOff  = juce::ImageFileFormat::loadFrom(BinaryData::SawOff_png,   BinaryData::SawOff_pngSize);
        sawButton.setImages(false, true, true, sawOff, 1.0f, {}, sawOn, 1.0f, {}, sawOn, 1.0f, {});
        addAndMakeVisible(sawButton);
        sawButton.onClick = [this]() { userSelect(1); };

        squareOn  = juce::ImageFileFormat::loadFrom(BinaryData::SquareOn_png,  BinaryData::SquareOn_pngSize);
        squareOff = juce::ImageFileFormat::loadFrom(BinaryData::SquareOff_png, BinaryData::SquareOff_pngSize);
        squareButton.setImages(false, true, true, squareOff, 1.0f, {}, squareOn, 1.0f, {}, squareOn, 1.0f, {});
        addAndMakeVisible(squareButton);
        squareButton.onClick = [this]() { userSelect(2); };

        // Bind to parameter
        paramID = waveformType + waveformIndex;
        parameter = dynamic_cast<juce::AudioParameterChoice*>(apvtsRef.getParameter(paramID));
        jassert(parameter != nullptr);

        // Listen for changes from host/automation/preset load
        apvtsRef.addParameterListener(paramID, this);

        // Initialise UI from current parameter value
        setWaveformFromIndex(parameter->getIndex());
    }

    ~WaveformSelector() override
    {
        if (parameter != nullptr)
            apvtsRef.removeParameterListener(paramID, this);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        auto buttonWidth = area.getWidth() / 3;
        sineButton .setBounds(area.removeFromLeft(buttonWidth));
        sawButton  .setBounds(area.removeFromLeft(buttonWidth));
        squareButton.setBounds(area);
    }

private:
    // --- Parameter listening ---
    void parameterChanged(const juce::String& changedParamID, float newValue) override
    {
        if (changedParamID != paramID) return;

        // We're on the audio thread here -> bounce to message thread to touch UI
        const int newIndex = static_cast<int>(newValue + 0.5f); // for choice params, newValue is the *index* (not normalized)
        juce::MessageManager::callAsync([this, newIndex]
        {
            setWaveformFromIndex(newIndex); // UI only, no param write
        });
    }

    // Called when user clicks a button
    void userSelect(int index)
    {
        if (parameter == nullptr) return;

        // Update UI
        setWaveformFromIndex(index);

        // Write parameter using normalized value
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(index));
        parameter->endChangeGesture();
    }

    // --- UI helpers (no parameter writes here) ---
    void setWaveformFromIndex(int index)
    {
        applySelectionUI(index == 0, index == 1, index == 2);
    }

    void applySelectionUI(bool sineOnSel, bool sawOnSel, bool squareOnSel)
    {
        auto set = [](juce::ImageButton& b, const juce::Image& on, const juce::Image& off, bool isOn)
        {
            b.setImages(false, true, true,
                        isOn ? on : off, 1.0f, {},
                        isOn ? on : off, 1.0f, {},
                        isOn ? on : off, 1.0f, {});
        };

        set(sineButton,   sineOn,   sineOff,   sineOnSel);
        set(sawButton,    sawOn,    sawOff,    sawOnSel);
        set(squareButton, squareOn, squareOff, squareOnSel);
    }

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::String paramID;
    juce::AudioParameterChoice* parameter = nullptr;

    juce::ImageButton sineButton, sawButton, squareButton;
    juce::Image sineOn, sineOff, sawOn, sawOff, squareOn, squareOff;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformSelector)
};


//
//class WaveformSelector : public juce::Component, private juce::AudioProcessorValueTreeState::Listener
//{
//public:
//    
//    WaveformSelector(juce::AudioProcessorValueTreeState& apvts, juce::String waveformType, juce::String waveformIndex)
//    {
//        // Sine button
//        sineOn = juce::ImageFileFormat::loadFrom(BinaryData::SineOn_png, BinaryData::SineOn_pngSize);
//        sineOff = juce::ImageFileFormat::loadFrom(BinaryData::SineOff_png, BinaryData::SineOff_pngSize);
//        sineButton.setImages(false, true, true, sineOff, 1.0f, {}, sineOn, 1.0f, {}, sineOn, 1.0f, {});
//        addAndMakeVisible(sineButton);
//        sineButton.onClick = [this]() { selectButton(&sineButton); };
//
//        // Saw button
//        sawOn = juce::ImageFileFormat::loadFrom(BinaryData::SawOn_png, BinaryData::SawOn_pngSize);
//        sawOff = juce::ImageFileFormat::loadFrom(BinaryData::SawOff_png, BinaryData::SawOff_pngSize);
//        sawButton.setImages(false, true, true, sawOff, 1.0f, {}, sawOn, 1.0f, {}, sawOn, 1.0f, {});
//        addAndMakeVisible(sawButton);
//        sawButton.onClick = [this]() { selectButton(&sawButton); };
//
//        // Square button
//        squareOn = juce::ImageFileFormat::loadFrom(BinaryData::SquareOn_png, BinaryData::SquareOn_pngSize);
//        squareOff = juce::ImageFileFormat::loadFrom(BinaryData::SquareOff_png, BinaryData::SquareOff_pngSize);
//        squareButton.setImages(false, true, true, squareOff, 1.0f, {}, squareOn, 1.0f, {}, squareOn, 1.0f, {});
//        addAndMakeVisible(squareButton);
//        squareButton.onClick = [this]() { selectButton(&squareButton); };
//        
//        auto paramID = waveformType + waveformIndex;
//        parameter = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramID));
////        bindToParameter(apvts, paramID);
//
//        selectButton(&sineButton);
//        apvts.addParameterListener(paramID, this);
//
//    }
//
////    void bindToParameter(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
////    {
////        parameter = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramID));
////        jassert(parameter != nullptr);
////        apvts.addParameterListener(paramID, this);
////
////        setWaveformFromIndex(parameter->getIndex());
////    }
//
//
//    void resized() override
//    {
//        auto area = getLocalBounds().reduced(10);
//        auto buttonWidth = area.getWidth() / 3;
//
//        sineButton.setBounds(area.removeFromLeft(buttonWidth));
//        sawButton.setBounds(area.removeFromLeft(buttonWidth));
//        squareButton.setBounds(area);
//    }
//
//
//private:
//    juce::ImageButton sineButton, sawButton, squareButton;
//    juce::Image sineOn, sineOff, sawOn, sawOff, squareOn, squareOff;
//    juce::String currentSelection;
//
//    juce::AudioParameterChoice* parameter = nullptr;
//
//    void selectButton(juce::ImageButton* selected)
//    {
//        auto set = [](juce::ImageButton& b, const juce::Image& on, const juce::Image& off, bool isOn)
//        {
//            b.setImages(false, true, true,
//                        isOn ? on : off, 1.0f, {},
//                        isOn ? on : off, 1.0f, {},
//                        isOn ? on : off, 1.0f, {});
//        };
//
//        set(sineButton, sineOn, sineOff, selected == &sineButton);
//        set(sawButton, sawOn, sawOff, selected == &sawButton);
//        set(squareButton, squareOn, squareOff, selected == &squareButton);
//
//        if (parameter != nullptr)
//        {
//            parameter->beginChangeGesture();
//            if (selected == &sineButton)   parameter->setValueNotifyingHost(0.0f); // index 0
//            if (selected == &sawButton)    parameter->setValueNotifyingHost(.5f);
//            if (selected == &squareButton) parameter->setValueNotifyingHost(1.0f);
//            parameter->endChangeGesture();
//        }
//    }
//
//    void setWaveformFromIndex(int index)
//    {
//        if      (index == 0) selectButton(&sineButton);
//        else if (index == 1) selectButton(&sawButton);
//        else if (index == 2) selectButton(&squareButton);
//    }
////
////    void parameterChanged(const juce::String&, float newValue) override
////    {
////        juce::MessageManager::callAsync([this, index = static_cast<int>(newValue)] {
////            setWaveformFromIndex(index);
////        });
////    }
//
//        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformSelector)
//};

// WaveformVisualizer.h/.cpp (or add near ComponentUtility if you prefer)
#pragma once
#include <JuceHeader.h>

class WaveformVisualizer : public juce::Component,
                           private juce::AudioProcessorValueTreeState::Listener,
                           private juce::Timer
{
public:
    WaveformVisualizer(juce::AudioProcessorValueTreeState& s, const juce::String& voicePrefix)
        : apvts(s), prefix(voicePrefix)
    {
        // Listen to all relevant params for this voice
        addParamListener("WAVEFORM"     + prefix);
        addParamListener("MOD_WAVEFORM" + prefix);
        addParamListener("MOD_RATIO_NUM"+ prefix);
        addParamListener("MOD_RATIO_DEN"+ prefix);
        addParamListener("MOD_INDEX"    + prefix);
        
        // Pre-allocate
        carrierBuf.resize(sampleCount);
        modBuf.resize(sampleCount);
        outBuf.resize(sampleCount);
        
        buildPreview();
        startTimerHz(30); // light refresh for smooth UI
        setOpaque(false);
        voiceIndex = voicePrefix.getIntValue()-1;
        addParamListener(PID::seqCurrentStep[voiceIndex]);

        for (int s = 0; s < 8; ++s) // or however many steps you have
            addParamListener(PID::seqStep[voiceIndex][s]);

        // optional: if modulation can be toggled on/off
        addParamListener(PID::MODAMOUNT_mod[voiceIndex]);
    }

    ~WaveformVisualizer() override
    {
        stopTimer();
        for (auto& id : listenedIDs)
            apvts.removeParameterListener(id, this);
    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::transparentBlack);

        auto r = getLocalBounds().reduced(8);
        drawOverlayScope(g, r, carrierBuf, modBuf, outBuf);
    }

    static float mapSampleY(float v, const juce::Rectangle<int>& plot)
    {
        if (!std::isfinite(v)) return std::numeric_limits<float>::quiet_NaN();
        return juce::jmap(v, -1.0f, 1.0f,
                          (float) plot.getBottom(),
                          (float) plot.getY());    }

    void drawOverlayScope(juce::Graphics& g,
                          juce::Rectangle<int> r,
                          const std::vector<float>& carrier,
                          const std::vector<float>& mod,
                          const std::vector<float>& out)
    {
        // Frame, midline, labels
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRect(r, 1);
        auto plot = r.reduced(6, 6);
        const float midY = plot.getY() + plot.getHeight() * 0.5f;
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawLine((float)plot.getX(), midY, (float)plot.getRight(), midY);

        // Helper to draw one vector mapped across full width
        auto drawPath = [&](const std::vector<float>& data, juce::Colour colour)
        {
            if (data.size() < 2) return;
            juce::Path p;
            const int N = (int)data.size();
            const float w = (float)plot.getWidth();
            const float x0 = (float)plot.getX();
            const float step = (N > 1) ? (w - 1.0f) / (float)(N - 1) : w;

            p.startNewSubPath(x0, mapSampleY(data[0], plot));
            for (int i = 1; i < N; ++i)
            {
                const float x = x0 + step * (float)i;
                const float y = mapSampleY(data[i], plot);
                p.lineTo(x, y);
            }
            g.setColour(colour.withAlpha(0.95f));
            g.strokePath(p, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        };

        // Draw order: mod (under), carrier, FM out (on top)
        drawPath(mod,     juce::Colours::mediumpurple);
        drawPath(carrier, juce::Colours::skyblue);
        drawPath(out,     juce::Colours::limegreen);

        // Tiny legend
//        auto legend = r.removeFromTop(18);
//        g.setFont(12.0f);
//        auto dot = [&](juce::Colour c, const juce::String& text, int x)
//        {
//            g.setColour(c); g.fillEllipse((float)x, (float)legend.getY()+4, 8.0f, 8.0f);
//            g.setColour(juce::Colours::white.withAlpha(0.8f));
//            g.drawText(text, x + 12, legend.getY()+1, 70, legend.getHeight(), juce::Justification::centredLeft);
//        };
//        int x = legend.getX() + 6;
//        dot(juce::Colours::mediumpurple, "Mod", x); x += 78;
//        dot(juce::Colours::skyblue,      "Carrier", x); x += 90;
//        dot(juce::Colours::limegreen,    "FM Out", x);
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::String prefix;
    std::vector<float> carrierBuf, modBuf, outBuf;
    std::vector<juce::String> listenedIDs;
    int voiceIndex;
    // Visualization settings
    static constexpr int sampleCount = 1024;      // screen resolution for curves
    static constexpr float carrierCycles = 2.5f;  // how many carrier periods across the width
    static constexpr float twoPi = juce::MathConstants<float>::twoPi;

    // Cached params (updated on timer)
    int carrierWaveIdx = 0;   // 0=Sine,1=Saw,2=Square
    int modWaveIdx     = 0;
    int num            = 1;
    int den            = 1;
    float modIndex     = 0.0f;  // your param is 0..50; here it’s treated as radians directly (feel free to rescale)

    void addParamListener(const juce::String& id)
    {
        listenedIDs.push_back(id);
        apvts.addParameterListener(id, this);
    }

    // ======= DSP helpers =======
    static inline float wrap01(float x)          { x -= std::floor(x); return x; }
    static inline float phasor(float phase)      { return wrap01(phase / twoPi); }

    static float oscSample(int typeIndex, float phase)
    {
        // typeIndex: 0=Sine, 1=Saw, 2=Square
        float t = phasor(phase);     // [0,1)
        switch (typeIndex)
        {
            case 0: // Sine
                return std::sin(phase);
            case 1: // Saw (bipolar)
                return 2.0f * t - 1.0f;
            case 2: // Square (50% duty)
                return (t < 0.5f) ? 1.0f : -1.0f;
            default:
                return std::sin(phase);
        }
    }

    void buildPreview()
    {
        // Pull current values atomically from APVTS
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("WAVEFORM" + prefix)))
            carrierWaveIdx = p->getIndex();
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("MOD_WAVEFORM" + prefix)))
            modWaveIdx = p->getIndex();
        if (auto* p = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("MOD_RATIO_NUM" + prefix)))
            num = juce::jmax(1, p->get());
        if (auto* p = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("MOD_RATIO_DEN" + prefix)))
            den = juce::jmax(1, p->get());
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("MOD_INDEX" + prefix)))
            modIndex = p->get(); // treat as radians; rescale if you prefer

        float modOn = *apvts.getRawParameterValue(PID::MODAMOUNT_mod[voiceIndex]);
        float modOffset = 0;
        if (modOn){
            const int currentStep = (int) pfloat(PID::seqCurrentStep[voiceIndex]);
            const int clampedStep = juce::jlimit(0, 7, currentStep);

            // step value (0..1)
            const float stepValue = pfloat(PID::seqStep[voiceIndex][clampedStep]);
            modOffset = stepValue*50;
        }

        // Normalized time over [0, 1). We’ll draw carrierCycles across the width
        // Carrier angular freq (in “cycles-per-window” units):
        const float wc = twoPi * carrierCycles;
        const float wm = wc * (static_cast<float>(num) / static_cast<float>(den)); // ratio w.r.t carrier

        float carPhase, modPhase, pmPhase;
        
        const float baseI    = modIndex;         // read from APVTS
        const float step     = modOffset;        // your stepValue or whatever (0..1)
        const float depthRad = baseI * 0.5f;     // choose a depth scale you like
        const float Ieff     = juce::jlimit(0.0f, 50.0f, baseI + step);

//        DBG("baseI=" << baseI
//            << " step=" << step
//            << " Ieff=" << Ieff);
        
        for (int n = 0; n < sampleCount; ++n)
        {
            float t = static_cast<float>(n) / static_cast<float>(sampleCount); // 0..1
            carPhase = wc * t;
            modPhase = wm * t;

            const float m = oscSample(modWaveIdx, modPhase);       // [-1, 1]
            const float c = oscSample(carrierWaveIdx, carPhase);   // unmodded carrier


            pmPhase = carPhase + (Ieff * m);

            const float yOut = oscSample(carrierWaveIdx, pmPhase);

            carrierBuf[n] = c;
            modBuf[n]     = m;
            outBuf[n]     = yOut;
        }
    }


    // APVTS listener
    void parameterChanged(const juce::String&, float) override
    {
        // Just mark dirty; we rebuild on the timer thread (message thread via repaint)
        needsRebuild = true;
    }

    void timerCallback() override
    {
        if (needsRebuild.exchange(false))
        {
            buildPreview();
            repaint();
        }
    }

    std::atomic<bool> needsRebuild { true };
    
    float pfloat(const juce::String& id) const
    {
        if (auto* p = apvts.getRawParameterValue(id)) return p->load();
        return 0.0f;
    }

    int seqIndexFromPrefix() const
    {
        // if prefix is "1","2","3","4"... -> map to 0..N-1
        const int raw = prefix.getIntValue();
        // clamp to your voice count (here assuming 1..4); change 4 to your max voices
        const int idx = juce::jlimit(1, 4, raw) - 1;
        return idx;
    }
    float modIndexDepth = 0.0f;   // how much the step adds (you can surface a param for this)
    bool  modIndexFlag  = true;
};
