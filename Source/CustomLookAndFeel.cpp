
#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, JUCE_LIVE_CONSTANT (juce::Colour (0xFFFF00))); // dark slate-like
}

CustomLookAndFeel::~CustomLookAndFeel()
{
    
}

void CustomLookAndFeel::drawBubble(juce::Graphics& g,
                                   juce::BubbleComponent& bubble,
                                   const juce::Point<float>& tip,
                                   const juce::Rectangle<float>& body)
{
    // Example: flat rounded bubble with triangle
//    bubble.setBounds(body.getX(),body.getY(),body.getWidth(),20);
    
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
 {
     const auto bounds   = juce::Rectangle<float>(x, y, width, height).toFloat();
     const float lineW   = 2.0f;
     const float radius  = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f - (lineW + 2.0f);
     const auto  centre  = bounds.getCentre();
     const float cx      = centre.x, cy = centre.y;

     const float angleRange = rotaryEndAngle - rotaryStartAngle;
     const float angle      = rotaryStartAngle + sliderPos * angleRange;

     // Accent palette
     const bool  modOn     = (isModulated != 0);
     const auto  accentB   = modOn ? juce::Colour::fromRGB(32,0,63) : juce::Colour::fromRGB(64,16,128);
     const auto  accentA   = modOn ? juce::Colour::fromRGB(16, 0,64) : juce::Colour::fromRGB(32, 0,96);

     // ---------------- Background arc ----------------
     {
         juce::Path bg;
         bg.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
         g.setColour(juce::Colours::darkgrey.darker(0.6f));
         g.strokePath(bg, juce::PathStrokeType(lineW));
     }

     // ---------------- Value arc ----------------
     {
         juce::Colour arcColour = accentA.interpolatedWith(accentB, sliderPos);
         juce::Path arc; arc.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, angle, true);
         g.setColour(arcColour.withAlpha(0.95f));
         g.strokePath(arc, juce::PathStrokeType(lineW + 0.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
     }

     // ---------------- Center glow (emanates with value) ----------------
     {
         const float glowAmt = juce::jlimit(0.0f, 1.0f, sliderPos);
         juce::Colour inner  = accentA.withAlpha(0.20f + 0.40f * glowAmt);
         juce::Colour mid    = accentB.withAlpha(0.12f * glowAmt);
         juce::Colour outer  = juce::Colours::transparentBlack;

         juce::ColourGradient cg(inner, cx, cy, outer, cx, cy, true);
         cg.addColour(0.40, inner);
         cg.addColour(0.80, mid);
         g.setGradientFill(cg);

         const float glowR = radius * (0.60f + 0.60f * glowAmt);
         g.fillEllipse(cx - glowR, cy - glowR, glowR * 2.0f, glowR * 2.0f);
     }

     // ---------------- Knob face (flat) ----------------
//     {
//         const float faceR = radius * 0.78f;
//         g.setColour(juce::Colour::fromRGB(32  , 32, 32));
//         g.fillEllipse(cx - faceR, cy - faceR, faceR * 2.0f, faceR * 2.0f);
//     }

     // ---------------- Pointer ----------------
    // ----- Pointer that starts at the face edge, leaving the centre blank -----
    {
        // if you have these earlier, reuse them:
        const float lineW   = 2.0f;                 // your track stroke thickness
        const float faceR   = radius * 0.5f;       // same as your knob face radius
        const float gap     = 2.0f;                 // small gap between face and pointer (optional)
        const float rInner  = faceR + gap;          // start of pointer (away from centre)
        const float rOuter  = radius - lineW * 0.5f;// end of pointer (near the value arc)

        float pointerLen = rOuter - rInner;
        if (pointerLen > 0.0f)
        {
            const float pointerTh = (isModulated ? 2.6f : 2.2f);

            juce::Path pointer;
            // rectangle whose top touches rOuter and bottom touches rInner (in the unrotated Y- axis)
            pointer.addRoundedRectangle(-pointerTh * 0.5f, -rOuter, pointerTh, pointerLen, pointerTh * 0.5f);

            g.setColour(juce::Colours::lightgrey.withAlpha(0.95f));
            g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(cx, cy));
        }
    }
 }
//void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
//                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
//                                          juce::Slider& slider)
// {
//     const auto bounds = juce::Rectangle<float>(x, y, width, height);
//     const float lineWidth = 3.0f;
//     const float radius = juce::jmin(width, height) / 2.0f - lineWidth;
//     const float centerX = bounds.getCentreX();
//     const float centerY = bounds.getCentreY();
//     const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
//
//     // Background arc
//     juce::Path backgroundArc;
//     backgroundArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
//     g.setColour(juce::Colours::darkgrey.darker(0.7f)); // softer dark
//     g.strokePath(backgroundArc, juce::PathStrokeType(lineWidth));
//
//     // Value arc with gradient
//     juce::Colour startColour = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f);  // warm grey
//     juce::Colour endColour = juce::Colour::fromFloatRGBA(0.9f, 0.5f, 0.2f, 1.0f);    // soft orange
//
//     auto interpolated = startColour.interpolatedWith(endColour, sliderPos);
//     g.setColour(interpolated);
//     juce::Path valueArc;
//     valueArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
//     g.strokePath(valueArc, juce::PathStrokeType(lineWidth + 0.5f));
//
//     // Subtle glow
////     g.setColour(interpolated.withAlpha(0.15f));
////     g.fillEllipse(bounds.reduced(width * 0.15f));
//
//     // Inner circle (center of knob)
//    if (isModulated==0){
//        g.setColour(juce::Colours::darkgrey.brighter(0.1));
//    }
//    else{
//        g.setColour(juce::Colours::blueviolet.brighter(0.1f));
//    }
//    
//     g.fillEllipse(centerX - radius * 0.6f, centerY - radius * 0.6f, radius * 1.2f, radius * 1.2f);
//
//     // Pointer
//     juce::Path pointer;
//     const float pointerLength = radius * 0.9f;
//     const float pointerThickness = 2.0f;
//     pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
//     g.setColour(juce::Colours::lightgrey); // softer than white
//     g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centerX, centerY));
// }

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          const juce::Slider::SliderStyle style, juce::Slider& slider)
 {
     const float trackThickness = 7.0f;
     const float thumbRadius = 7.0f;

     if (style == juce::Slider::LinearVertical)
     {
         float centerX = x + width / 2.0f;
         float trackX = centerX - trackThickness / 2.0f;
         float trackY = (float)y;
         float trackHeight = (float)height;

         // Background track
         juce::Rectangle<float> trackBounds(trackX, trackY, trackThickness, trackHeight);
//         g.setColour(juce::Colour::fromFloatRGBA(0.25f, 0.25f, 0.25f, 1.0f));
//         g.fillRoundedRectangle(trackBounds, trackThickness / 2.0f);

         // Filled area
         float fillTop = juce::jmin(sliderPos, maxSliderPos);
         float fillBottom = juce::jmax(sliderPos, minSliderPos);
         juce::Rectangle<float> fillBounds(trackX, fillTop, trackThickness, fillBottom - fillTop);
         g.setColour(juce::Colour::fromFloatRGBA(0.75f, 0.75f, 0.75f, 1.0f));
         g.fillRoundedRectangle(fillBounds, trackThickness / 2.0f);

         // Glow behind thumb
         g.setColour(juce::Colour::fromFloatRGBA(0.75f, 0.75f, 0.75f, 0.2f));
         g.fillEllipse(centerX - thumbRadius * 1.2f, sliderPos - thumbRadius * 1.2f, thumbRadius * 2.4f, thumbRadius * 2.4f);

         // Thumb
         g.setColour(juce::Colour::fromFloatRGBA(0.5f, 0.5f, 1.0f, 1.0f)); // soft blue
         g.fillEllipse(centerX - thumbRadius, sliderPos - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
     }
     else if (style == juce::Slider::LinearHorizontal)
     {
         float centerY = y + height / 2.0f;
         float trackY = centerY - trackThickness / 2.0f;
         float trackX = (float)x;
         float trackWidth = (float)width;

         // Background track
         juce::Rectangle<float> trackBounds(trackX, trackY, trackWidth, trackThickness);
//         g.setColour(juce::Colour::fromFloatRGBA(0.25f, 0.25f, 0.25f, 1.0f));
//         g.fillRoundedRectangle(trackBounds, trackThickness / 2.0f);

         // Filled area
         float fillLeft = juce::jmin(sliderPos, maxSliderPos);
         float fillRight = juce::jmax(sliderPos, minSliderPos);
         juce::Rectangle<float> fillBounds(fillLeft, trackY, fillRight - fillLeft, trackThickness);
         g.setColour(juce::Colour::fromFloatRGBA(0.75f, 0.75f, 0.75f, 1.0f));
         g.fillRect(fillBounds);

////         // Glow behind thumb
//         g.setColour(juce::Colour::fromFloatRGBA(0.75f, 0.75f, 0.75f, 0.2f));
//         g.fillEllipse(sliderPos - thumbRadius * 1.2f, centerY - thumbRadius * 1.2f, thumbRadius * 2.4f, thumbRadius * 2.4f);

         // Thumb
         g.setColour(juce::Colour::fromFloatRGBA(0.5f, 0.5f, 1.0f, 1.0f));
         // Quantize the sliderPos to 8 steps
         int totalSteps = 8;
         float normalized = juce::jlimit(0.0f, 1.0f, (sliderPos - minSliderPos) / (maxSliderPos - minSliderPos));
         int stepIndex = static_cast<int>(std::round(normalized * (totalSteps )));
         float quantizedNorm = stepIndex / float(totalSteps );
         float quantizedSliderPos = minSliderPos + quantizedNorm * (maxSliderPos - minSliderPos);

         // Now draw using the quantized position
//         g.fillEllipse(quantizedSliderPos - thumbRadius, centerY - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
     }
 }

StepSequencerLookAndFeel::StepSequencerLookAndFeel(){
    
}

StepSequencerLookAndFeel::~StepSequencerLookAndFeel(){
    
}
