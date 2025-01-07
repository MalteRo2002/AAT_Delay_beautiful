#include <juce_audio_processors/juce_audio_processors.h>

class InfoPopup : public juce::Component
{
public:
    InfoPopup()
    {
        setAlwaysOnTop(true); // Sicherstellen, dass das Popup immer oben angezeigt wird
        setSize(100, 30);     // Standardgröße des Popups
    }

    void setText(const juce::String& text)
    {
        displayText = text;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawFittedText(displayText, getLocalBounds(), juce::Justification::centred, 1);
    }

private:
    juce::String displayText; // Text, der im Popup angezeigt wird
};


class MouseButtonDetectOverlay : public juce::Slider
{
public:
    MouseButtonDetectOverlay(juce::Slider& leftSlider, juce::Slider& rightSlider, juce::AudioProcessorValueTreeState& vts) : m_leftSlider(leftSlider), m_rightSlider(rightSlider), m_apvts(vts)
    {
        popup.setVisible(false);
    }

    juce::String getParamName() { return m_param; }
    juce::String getUnitName() { return m_unit; }

    void setParamName(const juce::String& name) { m_param = name; }
    void setUnitName(const juce::String& unit) { m_unit = unit; }

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
            updatePopupText();
        }
        if (event.mods.isRightButtonDown())
        {
            m_rightSlider.mouseDrag(event); // Let the slider behave as normal
            updatePopupText();
        }
        juce::Slider::mouseDrag(event);
    }

    void mouseEnter(const juce::MouseEvent& event) override
    {
        if (auto* parent = getParentComponent())
        {
            parent->addChildComponent(popup);

            auto sliderBounds = getBounds();
            auto popupWidth = 150;
            auto popupHeight = 40;

            int popupX = sliderBounds.getCentreX() - (popupWidth / 2);
            int popupY = sliderBounds.getBottom();

            popup.setBounds(popupX, popupY, popupWidth, popupHeight);
            popup.setVisible(true);
            updatePopupText();
        }
    }

    void mouseExit(const juce::MouseEvent& event) override
    {
        popup.setVisible(false); // Popup ausblenden
        juce::Slider::mouseExit(event);
    }

    void updatePopupText()
    {
        if (m_apvts.getRawParameterValue("LinkLRID")->load())
        {
            popup.setText(m_param + juce::String(m_leftSlider.getValue(), 2) + m_unit);
        }
        else
        {
            popup.setText(
                "Left: " + juce::String(m_leftSlider.getValue(), 2) + m_unit + "\n" +
                "Right: " + juce::String(m_rightSlider.getValue(), 2) + m_unit);
        }
    }

    void setValueUnit(const juce::String& unit) { valueUnit = unit; }

    void paint(juce::Graphics& g) override
    {
        // Do not draw anything, making the slider invisible
    }


private:
    juce::Slider& m_leftSlider;
    juce::Slider& m_rightSlider;
    InfoPopup popup;
    juce::String valueUnit; 
    juce::AudioProcessorValueTreeState& m_apvts;
    juce::String m_param;
    juce::String m_unit;
};