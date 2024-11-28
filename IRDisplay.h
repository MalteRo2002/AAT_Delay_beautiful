#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "BasicDelayEffect.h"

class IRDisplay : public juce::Component
{
public:
    IRDisplay(juce::AudioProcessorValueTreeState& vts);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setDelay_msLeft(float delay_msLeft){m_delay_msLeft = delay_msLeft; 
    m_delay.setDelay_s(delay_msLeft*0.001f,0);repaint();};
    void setDelay_msRight(float delay_msRight){m_delay_msRight = delay_msRight;
    m_delay.setDelay_s(delay_msRight*0.001f,1);repaint();};
    void setFeedbackLeft(float feedbackLeft){m_feedbackLeft = feedbackLeft;
    m_delay.setFeedback(feedbackLeft,0);repaint();};
    void setFeedbackRight(float feedbackRight){m_feedbackRight = feedbackRight;
    m_delay.setFeedback(feedbackRight,1);repaint();};
    void setCrossFeedbackLeft(float crossFeedbackLeft){m_crossFeedbackLeft = crossFeedbackLeft;
    m_delay.setCrossFeedback(crossFeedbackLeft,0);repaint();};
    void setCrossFeedbackRight(float crossFeedbackRight){m_crossFeedbackRight = crossFeedbackRight;
    m_delay.setCrossFeedback(crossFeedbackRight,1);repaint();};
    void setBpm(float bpm){m_bpm = bpm;repaint();};
    void setScaleFactor(float scaleFactor){m_scaleFactor = scaleFactor;repaint();};
    void setDryWet(float dryWet){m_dryWet = dryWet;
    m_delay.setDryWet(dryWet); repaint();};
    void setFs(float fs){m_fs = fs; m_delay.setSamplerate(fs); repaint();};

    void setLowpassLeft(float lowpassLeft){m_lowpassLeft = lowpassLeft;
    m_delay.setLowpassFrequency(lowpassLeft,0);repaint();};
    void setLowpassRight(float lowpassRight){m_lowpassRight = lowpassRight;
    m_delay.setLowpassFrequency(lowpassRight,1);repaint();};
    void setHighpassLeft(float highpassLeft){m_highpassLeft = highpassLeft;
    m_delay.setHighpassFrequency(highpassLeft,0);repaint();};
    void setHighpassRight(float highpassRight){m_highpassRight = highpassRight;
    m_delay.setHighpassFrequency(highpassRight,1);repaint();};


private:
    void paintFunctional(juce::Graphics& g);
    void paintIR(juce::Graphics& g);
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
    float m_fs;
    float m_maxLen_s;
    int m_lenIR;

    float m_lowpassLeft;
    float m_lowpassRight;
    float m_highpassLeft;
    float m_highpassRight;

    juce::AudioBuffer <float> m_delta;
    jade::BasicDelayEffect m_delay;

    juce::ToggleButton m_showFunctional;
    bool m_showFunctionalState = false;


};