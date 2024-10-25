#include "BasicDelayLineTV.h"

jade::BasicDelayLine::BasicDelayLine()
{
    changeBufferSize();

}

void jade::BasicDelayLine::setDelay(size_t delay, size_t chn)
{
    if (delay >= m_maxdelay || chn >= m_nrOfChns)
        return;

    switch (m_algorithmswitch)
    {
    case switchAlgorithm::Digital:
        m_delays[chn] = static_cast<float>(delay);
        break;
    case switchAlgorithm::Fade:
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
        break;
    case switchAlgorithm::Tape:
        m_switchState[chn] = switchState::changeTime;
        m_incStep[chn] = static_cast<float>(delay - m_delays[chn])/m_switchTime;
        m_switchCounter[chn] = m_switchTime;
        break;
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
                int readPos = static_cast<int> (m_writePos) - static_cast<size_t>(m_delays[cc]);
                if (readPos < 0)
                    readPos += m_maxdelay;
            
                out = bufferPtr[cc][readPos];
            }
            else
            {
                if (m_algorithmswitch == switchAlgorithm::Fade)
                {
                    int readPos = static_cast<int> (m_writePos)  - static_cast<size_t>(m_delays[cc]);
                    if (readPos < 0)
                        readPos += m_maxdelay;

                    int readPosNew =static_cast<int> (m_writePos) - m_newdelays[cc];
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
                else // Tape
                {
                    m_delays[cc] += m_incStep[cc];
                    m_switchCounter[cc]--;
                    if (m_switchCounter[cc] == 0)
                    {
                        m_switchState[cc] = switchState::normal;
                        m_delays[cc] = static_cast<double>(static_cast<int> (m_delays[cc]+0.5));
                    }
                    float readPosFrac =static_cast<float> (m_writePos) - m_delays[cc];
                    if (readPosFrac<0.f)
                        readPosFrac += static_cast<float> (m_maxdelay);

                    int interp_x1 = static_cast<int> (readPosFrac);
                    float frac = readPosFrac - interp_x1;

                    int interp_x0 = interp_x1 - 1;
                    if (interp_x0 < 0)
                        interp_x0 += m_maxdelay;
                    
                    int interp_x2 = interp_x1 + 1;
                    if (interp_x2 >= m_maxdelay)
                        interp_x2 -= m_maxdelay;

                    int interp_x3 = interp_x1 + 2;
                    if (interp_x3 >= m_maxdelay)
                        interp_x3 -= m_maxdelay;

                    //float L_0 = - ((readPosFrac-interp_x1)*(readPosFrac-interp_x2)*(readPosFrac-interp_x3))/6.f;
                    //float L_1 = ((readPosFrac-interp_x0)*(readPosFrac-interp_x2)*(readPosFrac-interp_x3))/2.f;
                    //float L_2 =- ((readPosFrac-interp_x0)*(readPosFrac-interp_x1)*(readPosFrac-interp_x3))/2.f; 
                    //float L_3 = ((readPosFrac-interp_x0)*(readPosFrac-interp_x1)*(readPosFrac-interp_x2))/6.f;
                    float L_0 = - ((frac)*(frac-1.f)*(frac-2.f))/6.f;
                    float L_1 = ((frac+1.f)*(frac-1.f)*(frac-2.f))/2.f;
                    float L_2 =- ((frac+1.f)*(frac)*(frac-2.f))/2.f; 
                    float L_3 = ((frac+1.f)*(frac)*(frac-1.f))/6.f;

                    out = L_0*bufferPtr[cc][interp_x0] + L_1*bufferPtr[cc][interp_x1]
                            + L_2*bufferPtr[cc][interp_x2] + L_3*bufferPtr[cc][interp_x3];

                    if (out > 0.5 || out < -0.5)
                    {
                        DBG("Somethings wrong");
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
    m_buffer.clear();
    m_delays.resize(m_nrOfChns);
    m_newdelays.resize(m_nrOfChns);
    m_futuredelays.resize(m_nrOfChns);
    m_switchCounter.resize(m_nrOfChns);
    m_switchState.resize(m_nrOfChns);
    m_incStep.resize(m_nrOfChns);
    for (auto cc = 0; cc < m_nrOfChns; ++cc)
    {
        m_delays[cc] = 0;
        m_switchCounter[cc] = 0;
        m_newdelays[cc] = 0;
        m_futuredelays[cc] = 0;
        m_switchState[cc] = switchState::normal;
        m_incStep[cc] = 0.f;
    }
}

void jade::BasicDelayLine::resetSwitchStatus()
{
    for (auto cc = 0; cc < m_nrOfChns; ++cc)
    {
        if (m_switchState[cc] == switchState::changeTime)
        {
            m_delays[cc] = m_newdelays[cc];
        }
        if (m_switchState[cc] == switchState::futureValueSet)
        {
            m_delays[cc] = m_futuredelays[cc];
        }
        m_switchCounter[cc] = 0;
        m_newdelays[cc] = 0;
        m_futuredelays[cc] = 0;
        m_switchState[cc] = switchState::normal;
    }

}
