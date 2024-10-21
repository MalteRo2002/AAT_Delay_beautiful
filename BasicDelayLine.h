#pragma once

// Rahmenbedingungen
// JUCE nutzen ==> AudioBuffer 
// ncHns fähig

#include <vector>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace jade
{

class BasicDelayLine
{
public: 
    BasicDelayLine();
    void setMaxDelay(size_t maxdelay){m_maxdelay = maxdelay; changeBufferSize();};
    void setMaxDelay_s(float delay_s){size_t delay = static_cast<size_t> (delay_s*m_fs); setMaxDelay(delay);};
    void setNrOfChns(size_t chns){m_nrOfChns = chns; changeBufferSize();};
    void setSamplerate(float fs){m_fs = fs;};

    void setDelay(size_t delay, size_t chn);
    void setDelay_s(float delay_s, size_t chn){size_t delay = static_cast<size_t> (delay_s*m_fs); setDelay(delay, chn);};

    int processSamples(juce::AudioBuffer<float>& data);

private:
    void changeBufferSize();
    float m_fs;
    size_t m_maxdelay = 1000;
    size_t m_nrOfChns = 2;
    juce::AudioBuffer<float> m_buffer;

    size_t m_writePos = 0;
    std::vector<size_t> m_delays;

};



}