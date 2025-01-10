#include <juce_audio_processors/juce_audio_processors.h>

class InfoPopup : public juce::Component, private juce::Timer
{
public:
    InfoPopup()
    {
        setAlwaysOnTop(true);
        setSize(100, 30);
        setAlpha(0.0f);
    }

    void setText(const juce::String& text)
    {
        displayText = text;
        repaint();
    }

    void FadeIn(int startY, int targetY)
    {
        animatingIn = true;
        currentAlpha = 0.0f;
        setAlpha(currentAlpha);
        currentY = startY;
        finalY = targetY;
        setTopLeftPosition(getX(), currentY);
        setVisible(true);
        startTimer(10);
    }

    void FadeOut()
    {
        animatingIn = false;
        currentAlpha = 1.0f;
        setAlpha(currentAlpha);
        startTimer(10);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

        g.setGradientFill(juce::ColourGradient(
            juce::Colours::white.withAlpha(0.1f),
            0.0f, 0.0f,
            juce::Colours::transparentBlack,
            0.0f, static_cast<float>(getHeight()), false
        ));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawFittedText(displayText, getLocalBounds().reduced(5), juce::Justification::centred, 1);
    }

private:
    void timerCallback() override
    {
        const float alphaStep = 0.1f;
        const int positionStep = 3;

        if (animatingIn)
        {
            currentAlpha += alphaStep;
            currentY += (finalY - currentY) > positionStep ? positionStep : (finalY - currentY);

            if (currentAlpha >= 1.0f && currentY >= finalY)
            {
                currentAlpha = 1.0f;
                currentY = finalY;
                stopTimer();
            }
        }
        else
        {
            currentAlpha -= alphaStep;

            if (currentAlpha <= 0.0f)
            {
                currentAlpha = 0.0f;
                stopTimer();
                setVisible(false);
            }
        }

        setAlpha(currentAlpha);
        setTopLeftPosition(getX(), currentY);
        repaint();                    
    }

    juce::String displayText;
    float currentAlpha = 0.0f;
    int currentY = 0;
    int finalY = 0;
    bool animatingIn = true;
};


class MouseButtonDetectOverlay : public juce::Slider
{
public:
    MouseButtonDetectOverlay(juce::Slider& leftSlider, juce::Slider& rightSlider, juce::AudioProcessorValueTreeState& vts) : m_leftSlider(&leftSlider), m_rightSlider(&rightSlider), m_apvts(vts)
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
            m_leftSlider->mouseDown(event);
        }
        if (event.mods.isRightButtonDown())
        {
            m_rightSlider->mouseDown(event); 
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (m_leftSlider != nullptr && event.mods.isLeftButtonDown())
        {
            m_leftSlider->mouseDrag(event);
            updatePopupText();
        }
        if (m_leftSlider != nullptr && event.mods.isRightButtonDown())
        {
            m_rightSlider->mouseDrag(event);
            updatePopupText();
        }
        juce::Slider::mouseDrag(event);
    }

    void mouseEnter(const juce::MouseEvent&) override
    {
        if (auto* parent = getParentComponent())
        {
            parent->addChildComponent(popup);

            auto sliderBounds = getBounds();
            auto popupWidth = 150;
            auto popupHeight = 40;

            int popupX = sliderBounds.getCentreX() - (popupWidth / 2);
            int startY = sliderBounds.getBottom() - 20;
            int targetY = sliderBounds.getBottom() + 5;

            auto parentBounds = parent->getLocalBounds();
            popupX = juce::jlimit(parentBounds.getX(), parentBounds.getRight() - popupWidth, popupX);
            targetY = juce::jlimit(parentBounds.getY(), parentBounds.getBottom() - popupHeight, targetY);

            popup.setBounds(popupX, startY, popupWidth, popupHeight);
            popup.setVisible(true);
            popup.FadeIn(startY, targetY);
            updatePopupText();
        }
    }

    void mouseExit(const juce::MouseEvent& event) override
    {
        juce::Slider::mouseExit(event);
        popup.FadeOut();
    }

    void updatePopupText()
    {
        auto transformValue = [this](float value) -> float {
            if (m_param == "Lowpass" || m_param == "Highpass")
            {
                return expf(value);
            }
            else if (m_param == "Feedback" || m_param == "Cross Feedback")
            {
                return value * 100.0f;
            }
            return value;
        };

        if (m_apvts.getRawParameterValue("LinkLRID")->load() || m_param == "Dry / Wet" || m_param == "Switch Time")
        {
            float transformedValue = (m_leftSlider != nullptr) ? static_cast<float>(transformValue(m_leftSlider->getValue())) : 0.0f;

            popup.setText(m_param + ": " + juce::String(transformedValue, 2) + " " + m_unit);
        }
        else
        {
            float transformedLeft = (m_leftSlider != nullptr) ? static_cast<float>(transformValue(m_leftSlider->getValue())) : 0.0f;
            float transformedRight = (m_rightSlider != nullptr) ? static_cast<float>(transformValue(m_rightSlider->getValue())) : 0.0f;

            popup.setText(
                "Left: " + juce::String(transformedLeft, 2) + " " + m_unit + "\n" +
                "Right: " + juce::String(transformedRight, 2) + " " + m_unit);
        }
    }


    void setValueUnit(const juce::String& unit) { valueUnit = unit; }

    void paint(juce::Graphics& g) override
    {
        // Do not draw anything, making the slider invisible
    }


private:
    juce::AudioProcessorValueTreeState& m_apvts;
    juce::Slider* m_leftSlider;
    juce::Slider* m_rightSlider;
    InfoPopup popup;
    juce::String valueUnit; 
    juce::String m_param;
    juce::String m_unit;
};