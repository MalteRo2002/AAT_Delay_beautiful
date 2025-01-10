#include <juce_audio_processors/juce_audio_processors.h>

class LavaLookAndFeelLinkRight : public juce::LookAndFeel_V4
{
public:
    LavaLookAndFeelLinkRight()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::red);
    }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        (void)g;
        (void)x;
        (void)y;
        (void)width;
        (void)height;
        (void)sliderPos;
        (void)rotaryStartAngle;
        (void)rotaryEndAngle;
        //nothing
    }
};