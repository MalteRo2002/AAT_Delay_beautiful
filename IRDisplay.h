#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class IRDisplay : public juce::Component
{
public:
    IRDisplay(juce::AudioProcessorValueTreeState& vts):m_vts(vts)
    {
        m_delay_msLeft = 120.f;
        m_delay_msRight = 250.f;
        m_feedbackLeft = 0.5f;
        m_feedbackRight = 0.5f;
        m_crossFeedbackLeft = 0.3f;
        m_crossFeedbackRight = 0.3f;
        m_bpm = 120.f;
        m_scaleFactor = 1.f;
        m_dryWet = 0.5f;
    };

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setDelay_msLeft(float delay_msLeft){m_delay_msLeft = delay_msLeft;repaint();};
    void setDelay_msRight(float delay_msRight){m_delay_msRight = delay_msRight;repaint();};
    void setFeedbackLeft(float feedbackLeft){m_feedbackLeft = feedbackLeft;repaint();};
    void setFeedbackRight(float feedbackRight){m_feedbackRight = feedbackRight;repaint();};
    void setCrossFeedbackLeft(float crossFeedbackLeft){m_crossFeedbackLeft = crossFeedbackLeft;repaint();};
    void setCrossFeedbackRight(float crossFeedbackRight){m_crossFeedbackRight = crossFeedbackRight;repaint();};
    void setBpm(float bpm){m_bpm = bpm;repaint();};
    void setScaleFactor(float scaleFactor){m_scaleFactor = scaleFactor;repaint();};
    void setDryWet(float dryWet){m_dryWet = dryWet;repaint();};

private:
    void paintFunctional(juce::Graphics& g);
    juce::AudioProcessorValueTreeState& m_vts;
    float getDisplayRange(float maxdelay);
    float m_delay_msLeft;
    float m_delay_msRight;
    float m_feedbackLeft;
    float m_feedbackRight;
    float m_crossFeedbackLeft;
    float m_crossFeedbackRight;
    float m_bpm;
    float m_scaleFactor;
    float m_dryWet;

};