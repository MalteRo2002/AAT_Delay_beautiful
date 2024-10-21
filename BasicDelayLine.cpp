#include "BasicDelayLine.h"

jade::BasicDelayLine::BasicDelayLine()
{
    changeBufferSize();

}

void jade::BasicDelayLine::setDelay(size_t delay, size_t chn)
{
    if (delay < m_maxdelay && chn < m_nrOfChns)
    {
        m_delays[chn] = delay;
    }
    else
        return;
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
            int readPos = m_writePos - m_delays[cc];
            if (readPos < 0)
                readPos += m_maxdelay;
            
            out = bufferPtr[cc][readPos];
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
    for (auto cc = 0; cc < m_nrOfChns; ++cc)
    {
        m_delays[cc] = 0;
    }
}
