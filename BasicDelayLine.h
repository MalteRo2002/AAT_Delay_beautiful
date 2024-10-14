#pragma once

// Rahmenbedingungen
// 1. Mehrkanaligkeit der DelayLine
// 2. JUCE Audiobuffer nutzen
// 3. setter für Delay, setter für samplingrate, für die Anzahl der Kanäle, für die maximale Länge des Delays
// 4. blockprocessing 

#include <vector>

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace jade
{

class BasicDelayLine
{
public:    
    BasicDelayLine();
    int processSamples(juce::AudioBuffer<float> &data);

    void setSamplingrate(float fs){m_fs = fs; changeBufferSize();};
    void setNrOfChns(size_t chns){m_nrOfChns = chns; changeBufferSize();};
    void setMaxDelaySamples(size_t maxdelay){m_maxDelay = maxdelay; changeBufferSize();};
    void setMaxDelaySeconds(float maxdelay){m_maxDelay = static_cast<size_t> (maxdelay*m_fs); changeBufferSize();};

    void setDelaySamples(size_t delay, size_t chn);
    void setDelaySeconds(float delay, size_t chn);


private:
    void changeBufferSize();
    float m_fs = 44100.f;
    size_t m_nrOfChns = 2;
    size_t m_maxDelay = 44100;

    std::vector <size_t> m_delay;
    juce::AudioBuffer<float> m_buffer;
    size_t m_writePos = 0;
};


};
