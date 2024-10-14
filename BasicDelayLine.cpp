#include "BasicDelayLine.h"

jade::BasicDelayLine::BasicDelayLine()
{
    changeBufferSize();
}

int jade::BasicDelayLine::processSamples(juce::AudioBuffer<float> &data)
{
    int nrofchns = data.getNumChannels();
    int nrofsamples = data.getNumSamples();

    auto dataPtr = data.getArrayOfWritePointers();
    auto bufferPtr = m_buffer.getArrayOfWritePointers();

    for (size_t kk = 0 ; kk < nrofsamples; ++kk)
    {
        for (size_t chn = 0; chn < nrofchns; ++chn)
        {
            float in = dataPtr[chn][kk];
            bufferPtr[chn][m_writePos] = in;

            int readPos = static_cast<int> (m_writePos - m_delay[chn]);
            if (readPos<0)
                readPos += static_cast<int> (m_maxDelay);
            
            dataPtr[chn][kk] = bufferPtr[chn][readPos];
            // increase write pointer and check
            m_writePos++;
            if (m_writePos == m_maxDelay)
                m_writePos = 0;

        }
    }

    return 0;
}

void jade::BasicDelayLine::setDelaySamples(size_t delay, size_t chn)
{
    if (delay < m_maxDelay)
        m_delay[chn] = delay;
    else
        return;
}

void jade::BasicDelayLine::setDelaySeconds(float delay, size_t chn)
{
    size_t delay_samples = static_cast<size_t> (m_fs*delay);
    if (delay_samples < m_maxDelay)
        m_delay[chn] = delay_samples;
    else
        return;
}

void jade::BasicDelayLine::changeBufferSize()
{
    m_buffer.setSize(m_nrOfChns,m_maxDelay);
    m_buffer.clear();
    m_delay.resize(m_nrOfChns);
    for (size_t chn = 0; chn < m_nrOfChns; ++chn)
    {
        m_delay[chn] = 0;
    }
    m_writePos = 0;
}
