#include <juce_audio_processors/juce_audio_processors.h>

const struct
{
	const std::string ID = "SwitchAlgoID";
	const std::string name = "SwitchAlgo";
	const juce::StringArray choices = {"Digital","Fade","Tape"};
	const int defaultValue = 1;
}g_paramSwitchAlgo;

const struct
{
	const std::string ID = "CrossFeedbackLeftID";
	const std::string name = "CrossFeedbackLeft";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramCrossFeedbackLeft;

const struct
{
	const std::string ID = "CrossFeedbackRightID";
	const std::string name = "CrossFeedbackRight";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramCrossFeedbackRight;

const struct
{
	const std::string ID = "LinkLRID";
	const std::string name = "LinkLR";
	const bool defaultValue = true;
}g_paramLinkLR;

const struct
{
	const std::string ID = "SwitchTime_msID";
	const std::string name = "SwitchTime_ms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 500.f;
	const float defaultValue = 20.f;
}g_paramSwitchTime_ms;

class AdvancedPopup : public juce::Component, private juce::Timer
{
public:
    AdvancedPopup(juce::AudioProcessorValueTreeState& apvts, IRDisplay& IRDisplay) : m_apvts(apvts), m_IRDisplay(IRDisplay)
    {
        m_SwitchTime_msSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        m_SwitchTime_msSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
        m_SwitchTime_msSlider.setRange(g_paramSwitchTime_ms.minValue, g_paramSwitchTime_ms.maxValue);
        m_SwitchTime_msSlider.setTextValueSuffix(g_paramSwitchTime_ms.unitName);
        auto val = m_apvts.getRawParameterValue(g_paramSwitchTime_ms.ID);
        m_SwitchTime_msSlider.setValue(*val);
        m_SwitchTime_msAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramSwitchTime_ms.ID, m_SwitchTime_msSlider);
        addAndMakeVisible(m_SwitchTime_msSlider);

        m_CrossFeedbackLeftSlider.onValueChange = [this] {m_IRDisplay.setCrossFeedbackLeft(m_CrossFeedbackLeftSlider.getValue());};
        m_CrossFeedbackLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        m_CrossFeedbackLeftSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
        m_CrossFeedbackLeftSlider.setRange(g_paramCrossFeedbackLeft.minValue, g_paramCrossFeedbackLeft.maxValue);
        m_CrossFeedbackLeftSlider.setTextValueSuffix(g_paramCrossFeedbackLeft.unitName);
        val = m_apvts.getRawParameterValue(g_paramCrossFeedbackLeft.ID);
        m_CrossFeedbackLeftSlider.setValue(*val);
        m_CrossFeedbackLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramCrossFeedbackLeft.ID, m_CrossFeedbackLeftSlider);
        m_CrossFeedbackLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
        addAndMakeVisible(m_CrossFeedbackLeftSlider);

        m_CrossFeedbackRightSlider.onValueChange = [this] {m_IRDisplay.setCrossFeedbackRight(m_CrossFeedbackRightSlider.getValue());};
        m_CrossFeedbackRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        m_CrossFeedbackRightSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
        m_CrossFeedbackRightSlider.setRange(g_paramCrossFeedbackRight.minValue, g_paramCrossFeedbackRight.maxValue);
        m_CrossFeedbackRightSlider.setTextValueSuffix(g_paramCrossFeedbackRight.unitName);
        val = m_apvts.getRawParameterValue(g_paramCrossFeedbackRight.ID);
        m_CrossFeedbackRightSlider.setValue(*val);
        m_CrossFeedbackRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramCrossFeedbackRight.ID, m_CrossFeedbackRightSlider);
        m_CrossFeedbackRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
        addAndMakeVisible(m_CrossFeedbackRightSlider);

        m_CrossFeedbackOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        m_CrossFeedbackOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(m_CrossFeedbackOverlay);

        m_LinkLR.onClick = [this] {linkButtonClicked();};
        m_LinkLR.setButtonText("Link L/R");
        m_LinkLR.setToggleState(g_paramLinkLR.defaultValue, false);
        m_LinkLRAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(m_apvts, g_paramLinkLR.ID, m_LinkLR);
        addAndMakeVisible(m_LinkLR);

        m_AlgoSwitchCombo.addItemList(g_paramSwitchAlgo.choices,1);
        m_AlgoSwitchComboAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(m_apvts, g_paramSwitchAlgo.ID, m_AlgoSwitchCombo);
        addAndMakeVisible(m_AlgoSwitchCombo);
    }

void AdvancedPopup::FadeIn(int startY, int targetY)

    {
        animatingIn = true;
        currentAlpha = 0.0f;
        setAlpha(currentAlpha);
        currentY = startY;
        finalY = targetY;
        setVisible(true);
        startTimer(3);
    }

    void AdvancedPopup::FadeOut(int startY, int targetY)
    {
        animatingIn = false;
        currentAlpha = 1.0f;
        setAlpha(currentAlpha);
        currentY = startY;
        finalY = targetY;
        startTimer(3);
    }


    void paint(juce::Graphics& g) override
{
    g.setColour(juce::Colours::grey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f); // Abgerundetes Rechteck
}

    void resized() override
    {
        m_CrossFeedbackLeftSlider.setBounds(0,0,getWidth()*1/3,getHeight());
        m_CrossFeedbackRightSlider.setBounds(0,0, getWidth()*1/3,getHeight());
        m_CrossFeedbackOverlay.setBounds(0,0,getWidth()*1/3,getHeight());

        m_LinkLR.setBounds(getWidth()/2-getWidth()/7,getHeight()/3-getHeight()/8,getWidth()*1/3,getHeight()/4);

        m_SwitchTime_msSlider.setBounds(getWidth()*2/3,0,getWidth()*1/3,getHeight());
        
        m_AlgoSwitchCombo.setBounds(getWidth()/2-getWidth()/6,getHeight()*2/3-getHeight()/8,getWidth()*1/3,getHeight()/4);
    }

private:
    juce::AudioProcessorValueTreeState& m_apvts;

    juce::Slider m_CrossFeedbackLeftSlider;
	juce::Slider m_CrossFeedbackRightSlider;
	MouseButtonDetectOverlay m_CrossFeedbackOverlay{m_CrossFeedbackLeftSlider, m_CrossFeedbackRightSlider, m_apvts};
    juce::Slider m_SwitchTime_msSlider;
    juce::ToggleButton m_LinkLR;
    juce::ComboBox m_AlgoSwitchCombo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_SwitchTime_msAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_LinkLRAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_AlgoSwitchComboAttachment;

    LavaLookAndFeelLeft m_lavaLookAndFeelLeft;
	LavaLookAndFeelRight m_lavaLookAndFeelRight;
	LavaLookAndFeelLinkLeft m_lavaLookAndFeelLinkLeft;
	LavaLookAndFeelLinkRight m_lavaLookAndFeelLinkRight;

    IRDisplay& m_IRDisplay;

    void timerCallback() override
    {
        const float alphaStep = 0.2f;
        const int positionStep = 10;

        if (animatingIn)
        {
            currentAlpha += alphaStep;
            currentY -= (currentY - finalY) > positionStep ? positionStep : (currentY - finalY);

            if (currentAlpha >= 1.0f && currentY <= finalY)
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

    float currentAlpha = 0.0f;
    int currentY = 0;
    int finalY = 0;
    bool animatingIn = true;

    void linkButtonClicked()
    {
        if (m_apvts.getRawParameterValue(g_paramLinkLR.ID)->load())
        {
            m_CrossFeedbackLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
        }
        else
        {
            if(!m_apvts.getRawParameterValue(g_paramLinkLR.ID)->load())
            {
                m_CrossFeedbackLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLeft);
            }
        }
        if (m_apvts.getRawParameterValue(g_paramLinkLR.ID)->load())
        {
            m_CrossFeedbackRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
        }
        else
        {
            if(!m_apvts.getRawParameterValue(g_paramLinkLR.ID)->load())
            {
                m_CrossFeedbackRightSlider.setLookAndFeel(&m_lavaLookAndFeelRight);
            }
        }
    }
};