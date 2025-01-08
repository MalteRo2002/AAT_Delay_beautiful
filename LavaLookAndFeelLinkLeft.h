class LavaLookAndFeelLinkLeft : public juce::LookAndFeel_V4
{
public:
    LavaLookAndFeelLinkLeft()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::red);
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.8f));
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

        // Draw the pointer
        juce::Path p;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 4.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::lightgrey);
        g.fillPath(p);
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                          bool isMouseOverButton, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        auto edge = 4;
        buttonArea.reduce(edge, edge);

        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(buttonArea.toFloat(), 5.0f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(label.findColour(juce::Label::textColourId));

        g.setFont(juce::Font(14.0f, juce::Font::bold));

        g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
    }
};
