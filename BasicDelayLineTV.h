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
    enum class SwitchState
    {
        normal,
        switching,
        newDelay
    };
    enum class SwitchTechnique
    {
        hard,
        fade,
        time
    };
    enum class InterpolationMethod
    {
        none,
        linear,
        lagrange
    };
    
    BasicDelayLine();
    int processSamples(juce::AudioBuffer<float> &data);

    void setSamplingrate(float fs){m_fs = fs; changeBufferSize();};
    void setNrOfChns(size_t chns){m_nrOfChns = chns; changeBufferSize();};
    void setMaxDelaySamples(size_t maxdelay){m_maxDelay = maxdelay; changeBufferSize();};
    void setMaxDelaySeconds(float maxdelay){m_maxDelay = static_cast<size_t> (maxdelay*m_fs); changeBufferSize();};

    void setDelaySamples(size_t delay, size_t chn);
    void setDelaySeconds(float delay, size_t chn);

    void setSwitchTime(size_t switchtime){m_switchTime = switchtime; switchingChanged();};
    void setSwitchTimeSeconds(float switchtime){size_t time_samples = static_cast<size_t> (switchtime*m_fs); setSwitchTime(time_samples);};
    void setInterpolationMethod(InterpolationMethod method){m_interpMethod = method;};
    void setSwitchTechnique(SwitchTechnique technique){m_switchTechnique = technique; switchingChanged();};

private:
    void changeBufferSize();
    void switchingChanged();
    float m_fs = 44100.f;
    size_t m_nrOfChns = 2;
    size_t m_maxDelay = 44100;

    std::vector <size_t> m_delay;
    juce::AudioBuffer<float> m_buffer;
    size_t m_writePos = 0;

    size_t m_switchTime = 100;
    std::vector <size_t> m_switchCounter;
    std::vector <size_t> m_newdelay;
    std::vector <size_t> m_futuredelay;
    std::vector <SwitchState> m_switchState;

    SwitchTechnique m_switchTechnique = SwitchTechnique::fade;
    std::vector <float> m_gainInc;
    InterpolationMethod m_interpMethod = InterpolationMethod::lagrange;

};


};
