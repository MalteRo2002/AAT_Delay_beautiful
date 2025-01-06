#include <juce_audio_processors/juce_audio_processors.h>

class MouseButtonDetectOverlay : public juce::Slider
{
public:
    MouseButtonDetectOverlay(juce::Slider& leftSlider, juce::Slider& rightSlider) : m_leftSlider(leftSlider), m_rightSlider(rightSlider) {}

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (event.mods.isLeftButtonDown())
        {
            m_leftSlider.mouseDown(event); // Let the slider behave as normal
        }
        if (event.mods.isRightButtonDown())
        {
            m_rightSlider.mouseDown(event); // Let the slider behave as normal
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (event.mods.isLeftButtonDown())
        {
            m_leftSlider.mouseDrag(event); // Let the slider behave as normal
        }
        if (event.mods.isRightButtonDown())
        {
            m_rightSlider.mouseDrag(event); // Let the slider behave as normal
        }
    }
    void paint(juce::Graphics& g) override
    {
        // Do not draw anything, making the slider invisible
    }

private:
    juce::Slider& m_leftSlider;
    juce::Slider& m_rightSlider;
};