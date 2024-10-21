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
    void setNrOfChns(size_t chns){m_nrOfChns = chns; changeBufferSize();};
    void setDelay(size_t delay, size_t chns);

    int processSamples(juce::AudioBuffer<float>& data);

private:
    void changeBufferSize();
    size_t m_maxdelay = 1000;
    size_t m_nrOfChns = 2;
    juce::AudioBuffer<float> m_buffer;

    size_t m_writePos = 0;
    std::vector<size_t> m_delays;

};



}