#include "BasicDelayLineTV.h"

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
            float delayOut = 0.f;            
            bufferPtr[chn][m_writePos] = in;
            // read from buffer
            if (m_switchState[chn] == SwitchState::normal || m_switchTechnique == SwitchTechnique::hard)    
            {
                int readPos = static_cast<int> (m_writePos - m_delay[chn]);
                if (readPos<0)
                    readPos += static_cast<int> (m_maxDelay);
            
                delayOut = bufferPtr[chn][readPos];
            }
            else
            {
                switch (m_switchTechnique)
                {
                case SwitchTechnique::fade:
                    int readPos;
                    int newReadPos;
                    float gain;
                    readPos = static_cast<int>(m_writePos - m_delay[chn]);
                    newReadPos  = static_cast<int>(m_writePos - m_newdelay[chn]);
                    if (readPos < 0)
                    {
                        readPos += static_cast<int> (m_maxDelay);
                    }
                    if (newReadPos < 0)
                    {
                        newReadPos += static_cast<int> (m_maxDelay);
                    }
                    gain = static_cast<float> (m_switchCounter[chn]) * m_gainInc[chn];
                    delayOut = gain * bufferPtr[chn][readPos] + (1.f - gain) * bufferPtr[chn][newReadPos];
                    if (--m_switchCounter[chn] == 0)
                    {   
                        if (m_switchState[chn] == SwitchState::switching)
                        {
                            m_delay[chn] = m_newdelay[chn];
                            m_switchState[chn] = SwitchState::normal;
                        }
                        else
                        {
                            m_delay[chn] = m_newdelay[chn];
                            m_newdelay[chn] = m_futuredelay[chn];
                            m_switchCounter[chn] = m_switchTime;
                            m_switchState[chn] = SwitchState::switching;
                        }
                    }
                    break;

                case SwitchTechnique::time:
                    m_delay[chn] += m_gainInc[chn];
                    if (--m_switchCounter[chn] == 0)
                    {
                        m_switchState[chn] = SwitchState::normal;
                    }
                    int curReadPos; 
                    float curReadPosFrac =  m_writePos - m_delay[chn];
            
                    if (curReadPosFrac < 0)
                    {
                        curReadPos = static_cast<int>(m_writePos - m_delay[chn] + m_maxDelay);
                        curReadPosFrac += static_cast<float> (m_maxDelay);
                    }
                    else
                    {
                        curReadPos =static_cast<int>(m_writePos - m_delay[chn]);
                    }
                    if (curReadPos >= static_cast<int> (m_maxDelay))
                    {
                        curReadPos -= static_cast<int> (m_maxDelay);
                        curReadPosFrac -= static_cast<float> (m_maxDelay);
                    }
                    float frac = curReadPosFrac - static_cast<int> (curReadPosFrac);

                    // 0 = No interpolation, 1 = linear interpolation, 2 = lagrange interpolation

                    if (m_interpMethod == InterpolationMethod::none)
                    {
                        delayOut = bufferPtr[chn][curReadPos];
                    }
                    if (m_interpMethod == InterpolationMethod::linear) // linear interpolation
                    {
                        if (frac >= 0.f)
                        {
                            int nextReadPos = curReadPos + 1;
                            if (nextReadPos >= static_cast<int> (m_maxDelay))
                            {
                                nextReadPos = 0;
                            }
                            delayOut = (1.f - frac) * bufferPtr[chn][curReadPos] + frac * bufferPtr[chn][nextReadPos];
                        }
                    }
                    // lagrange interpolation 3. order
                    if (m_interpMethod == InterpolationMethod::lagrange)
                    {
                        int nextReadPos = curReadPos + 1;
                        if (nextReadPos >= static_cast<int> (m_maxDelay))
                        {
                            nextReadPos = 0;
                        }
                        int prevReadPos = curReadPos - 1;
                        if (prevReadPos < 0)
                        {
                            prevReadPos = static_cast<int> (m_maxDelay) - 1;
                        }
                        int nextnextReadPos = curReadPos + 2;
                        if (nextnextReadPos >= static_cast<int> (m_maxDelay))
                        {
                            nextnextReadPos -= static_cast<int> (m_maxDelay);
                        }

                        float IntPunkt = frac;
                        float L_0 = ((IntPunkt)*(IntPunkt -1)*(IntPunkt - 2)) / ((0 - 1)*(0 - 2)*(0 - 3));
                        float L_1 = ((IntPunkt + 1)*(IntPunkt - 1)*(IntPunkt - 2)) / ((1 - 0)*(1 - 2)*(1 - 3));
                        float L_2 = ((IntPunkt + 1)*(IntPunkt)*(IntPunkt - 2)) / ((2 - 0)*(2 - 1)*(2 - 3));
                        float L_3 = ((IntPunkt + 1)*(IntPunkt)*(IntPunkt - 1)) / ((3 - 0)*(3 - 1)*(3 - 2));

                        delayOut = bufferPtr[chn][prevReadPos] * L_0
                            + bufferPtr[chn][curReadPos] * L_1
                            + bufferPtr[chn][nextReadPos] * L_2
                            + bufferPtr[chn][nextnextReadPos] * L_3;
                    }
                    break;
                }
            }
            dataPtr[chn][kk] = delayOut;

        }
        // increase write pointer and check
        m_writePos++;
        if (m_writePos == m_maxDelay)
            m_writePos = 0;
    }

    return 0;
}

void jade::BasicDelayLine::setDelaySamples(size_t delay, size_t chn)
{
    if (delay < m_maxDelay)
    {   
        if (m_switchState[chn] == SwitchState::normal)
        {
            switch (m_switchTechnique)
            {
            case SwitchTechnique::hard:
                m_delay[chn] = delay;
                break;
            case SwitchTechnique::fade:
                m_newdelay[chn] = delay;
                m_switchState[chn] = SwitchState::switching;
                m_switchCounter[chn] = m_switchTime;
                m_gainInc[chn] = 1.0f / static_cast<float> (m_switchTime);
                break;
            case SwitchTechnique::time:
                m_gainInc[chn] = (m_newdelay[chn] - m_delay[chn]) / static_cast<float> (m_switchTime);
                m_switchState[chn] = SwitchState::switching;
                m_switchCounter[chn] = m_switchTime;
                break;
            }
        }
        else if (m_switchState[chn] == SwitchState::switching || m_switchState[chn] == SwitchState::newDelay)
        {
            if (m_switchTechnique == SwitchTechnique::fade)
            {
                m_futuredelay[chn] = delay;
                m_switchState[chn] = SwitchState::newDelay;
            }
        }
    }    
    else
        return;
}

void jade::BasicDelayLine::setDelaySeconds(float delay, size_t chn)
{
    size_t delay_samples = static_cast<size_t> (m_fs*delay);
    setDelaySamples(delay_samples, chn);
}

void jade::BasicDelayLine::changeBufferSize()
{
    m_buffer.setSize(m_nrOfChns,m_maxDelay);
    m_buffer.clear();
    m_delay.resize(m_nrOfChns);
    m_switchCounter.resize(m_nrOfChns);
    m_newdelay.resize(m_nrOfChns);
    m_futuredelay.resize(m_nrOfChns);
    m_switchState.resize(m_nrOfChns);
    for (size_t chn = 0; chn < m_nrOfChns; ++chn)
    {
        m_delay[chn] = 0;
        m_switchCounter[chn] = 0;
        m_newdelay[chn] = 0;
        m_futuredelay[chn] = 0;
        m_switchState[chn] = SwitchState::normal;
    }
    m_writePos = 0;
}

void jade::BasicDelayLine::switchingChanged()
{
    for (size_t chn = 0; chn < m_nrOfChns; ++chn)
    {
        m_switchCounter[chn] = 0;
        if (m_switchState[chn] == SwitchState::switching)
        {
            m_delay[chn] = m_newdelay[chn];
        }
        else if (m_switchState[chn] == SwitchState::newDelay)
        {
            m_delay[chn] = m_futuredelay[chn];
        }
        m_newdelay[chn] = m_delay[chn];
        m_futuredelay[chn] = m_delay[chn];
        m_switchState[chn] = SwitchState::normal;
    }

}

