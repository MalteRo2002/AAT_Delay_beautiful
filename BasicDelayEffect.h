#pragma once

// Rahmenbedingungen
// JUCE nutzen ==> AudioBuffer 
// ncHns fähig

#include <vector>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "FirstOrderFilter.h"

namespace jade
{

class BasicDelayEffect
{
public: 
    enum class switchState
    {
        normal,
        changeTime,
        futureValueSet,
    };
    enum class switchAlgorithm
    {
        digital,
        fade,
        tape
    };

    BasicDelayEffect();
    void setMaxDelay(size_t maxdelay){m_maxdelay = maxdelay; changeBufferSize();};
    void setMaxDelay_s(float delay_s){size_t delay = static_cast<size_t> (delay_s*m_fs); setMaxDelay(delay);};
    void setNrOfChns(size_t chns){m_nrOfChns = chns; changeBufferSize();};
    void setSamplerate(float fs);

    void setDelay(size_t delay, size_t chn);
    void setDelay_s(float delay_s, size_t chn){size_t delay = static_cast<size_t> (delay_s*m_fs); setDelay(delay, chn);};

    int processSamples(juce::AudioBuffer<float>& data);
    void setSwitchTime(size_t time){m_switchTime = time;};
    void setSwitchAlgorithm (switchAlgorithm algo){m_switchalgorithm = algo;switchalgorithmChanged();};

    // feedback
    void setFeedback(float feedback, size_t chn) {m_feedback[chn] = feedback;};
    void setCrossFeedback(float feedback, size_t chn) {m_Crossfeedback[chn] = feedback;};

    void setLowpassFrequency(float fcut);
    void setHighpassFrequency(float fcut);
    void setLowpassFrequency(float fcut, size_t chn){m_lowpass[chn].setCutoff(fcut);};
    void setHighpassFrequency(float fcut, size_t chn){m_highpass[chn].setCutoff(fcut);};
    void setDryWet(float drywet){m_drywet = drywet;};
private:
    void changeBufferSize();
    void switchalgorithmChanged();
    float m_fs;
    size_t m_maxdelay = 1000;
    size_t m_nrOfChns = 2;
    juce::AudioBuffer<float> m_buffer;

    size_t m_writePos = 0;
    std::vector<double> m_delays;

    switchAlgorithm m_switchalgorithm = switchAlgorithm::tape;
    size_t m_switchTime = 100;
    std::vector<size_t> m_switchCounter;
    std::vector<switchState> m_switchState;
    // Fade Switch
    std::vector<size_t> m_newdelays;
    std::vector<size_t> m_futuredelays;

    // tape switch
    std::vector <double> m_fadeInc;

    // feedback
    std::vector <float> m_feedback;
    std::vector <float> m_oldOut;
    std::vector <float> m_Crossfeedback;
    std::vector <FirstOrderFilter> m_lowpass;
    std::vector <FirstOrderFilter> m_highpass;

    float m_drywet;

};



}