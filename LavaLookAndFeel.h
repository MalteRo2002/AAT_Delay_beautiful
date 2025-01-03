#include <juce_audio_processors/juce_audio_processors.h>

class LavaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LavaLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::red);
    }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Fill the slider background with a gradient
        juce::ColourGradient backgroundGradient(juce::Colours::darkgrey, rx, ry,
                                                juce::Colours::black, rx + rw, ry + rw, false);
        g.setGradientFill(backgroundGradient);
        g.fillEllipse(rx, ry, rw, rw);

        // // Outline
        // g.setColour(juce::Colours::black);
        // g.drawEllipse(rx-10, ry-10, rw, rw, 5.0f);

        // Draw the pointer
        juce::Path p;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 4.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::orange);
        g.fillPath(p);
    }
};