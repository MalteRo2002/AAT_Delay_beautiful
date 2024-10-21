#include "BasicDelayLineTV.h"

jade::BasicDelayLine::BasicDelayLine()
{
    changeBufferSize();

}

void jade::BasicDelayLine::setDelay(size_t delay, size_t chn)
{
    if (delay >= m_maxdelay || chn >= m_nrOfChns)
        return;

    if (m_switchState[chn] == switchState::normal)
    {
        m_switchState[chn] = switchState::changeTime;
        m_newdelays[chn] = delay;
        m_switchCounter[chn] = m_switchTime;
    }
    else 
    {
        m_switchState[chn] = switchState::futureValueSet;
        m_futuredelays[chn] = delay;
    }

}

int jade::BasicDelayLine::processSamples(juce::AudioBuffer<float> &data)
{
    size_t nrofchns = data.getNumChannels();
    size_t nrofsamples = data.getNumSamples();

    auto dataPtr = data.getArrayOfWritePointers();
    auto bufferPtr = m_buffer.getArrayOfWritePointers();
    for (auto kk = 0; kk < nrofsamples; ++kk)
    {
        for (auto cc = 0; cc < nrofchns; ++cc)
        {
            float in = dataPtr[cc][kk];
            bufferPtr[cc][m_writePos] = in; 

            float out = 0.f;
            if (m_switchState[cc] == switchState::normal)
            {
                int readPos = m_writePos - m_delays[cc];
                if (readPos < 0)
                    readPos += m_maxdelay;
            
                out = bufferPtr[cc][readPos];
            }
            else
            {
                int readPos = m_writePos - m_delays[cc];
                if (readPos < 0)
                    readPos += m_maxdelay;

                int readPosNew = m_writePos - m_newdelays[cc];
                if (readPosNew < 0)
                    readPosNew += m_maxdelay;

                float gain;
                gain = static_cast<float>(m_switchCounter[cc])/m_switchTime;
                out = gain * bufferPtr[cc][readPos] + (1.f - gain) * bufferPtr[cc][readPosNew] ;
                --m_switchCounter[cc];
                if (m_switchCounter[cc] == 0)
                {
                    m_delays[cc] = m_newdelays[cc];
                    if (m_switchState[cc] == switchState::changeTime)
                    {
                        m_switchState[cc] = switchState::normal;
                    }
                    else
                    {
                        m_switchState[cc] = switchState::changeTime;
                        m_switchCounter[cc] = m_switchTime;
                        m_newdelays[cc] = m_futuredelays[cc];
                    }

                }

            }
            dataPtr[cc][kk] = out;
        }

        m_writePos++;
        if (m_writePos == m_maxdelay)
            m_writePos = 0;
    }

    return 0;
}

void jade::BasicDelayLine::changeBufferSize()
{
    m_buffer.setSize (static_cast<int>(m_nrOfChns), static_cast<int>(m_maxdelay));
    m_delays.resize(m_nrOfChns);
    m_newdelays.resize(m_nrOfChns);
    m_futuredelays.resize(m_nrOfChns);
    m_switchCounter.resize(m_nrOfChns);
    m_switchState.resize(m_nrOfChns);
    for (auto cc = 0; cc < m_nrOfChns; ++cc)
    {
        m_delays[cc] = 0;
        m_switchCounter[cc] = 0;
        m_newdelays[cc] = 0;
        m_futuredelays[cc] = 0;
        m_switchState[cc] = switchState::normal;
    }
}
