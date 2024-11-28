#include "BasicDelayEffect.h"

jade::BasicDelayEffect::BasicDelayEffect()
{
    changeBufferSize();
    for (size_t kk = 0; kk < m_nrOfChns; kk++)
    {
        m_lowpass[kk].setDesignroutine(FirstOrderFilter::FilterDesign::lowpassButter);
        m_highpass[kk].setDesignroutine(FirstOrderFilter::FilterDesign::highpassButter);
    }

}

void jade::BasicDelayEffect::setSamplerate(float fs)
{
    m_fs = fs;
    for (size_t kk = 0; kk < m_nrOfChns; kk++)
    {
        m_lowpass[kk].setSamplerate(m_fs);
        m_highpass[kk].setSamplerate(m_fs);
    }
}

void jade::BasicDelayEffect::setDelay(size_t delay, size_t chn)
{
    if (delay >= m_maxdelay || chn >= m_nrOfChns)
        return;

    switch (m_switchalgorithm)
    {
    case switchAlgorithm::digital:
        m_delays[chn] = static_cast<double> (delay);
        break;
    case switchAlgorithm::fade:
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
    case switchAlgorithm::tape:
        m_switchState[chn] = switchState::changeTime;
        m_switchCounter[chn] = m_switchTime;
        m_fadeInc[chn] = (static_cast<double> (delay) - m_delays[chn])/m_switchTime;

        break;

    }
}

int jade::BasicDelayEffect::processSamples(juce::AudioBuffer<float> &data)
{
    size_t nrofchns = data.getNumChannels();
    size_t nrofsamples = data.getNumSamples();

    auto dataPtr = data.getArrayOfWritePointers();
    auto bufferPtr = m_buffer.getArrayOfWritePointers();
    for (auto kk = 0; kk < nrofsamples; ++kk)
    {
        for (auto cc = 0; cc < nrofchns; ++cc)
        {
            float in;
            if (cc == 0 && nrofchns == 2)
                in = dataPtr[cc][kk] + m_feedback[cc]*m_oldOut[cc] + m_Crossfeedback[1]*m_oldOut[1];

            if (cc == 1)
                in = dataPtr[cc][kk] + m_feedback[cc]*m_oldOut[cc] + m_Crossfeedback[0]*m_oldOut[0];

            
            bufferPtr[cc][m_writePos] = in; 

            float out = 0.f;
            if (m_switchState[cc] == switchState::normal)
            {
                int readPos = static_cast<int> (m_writePos) - static_cast<int> (m_delays[cc]);
                if (readPos < 0)
                    readPos += static_cast<int> (m_maxdelay);
            
                out = bufferPtr[cc][readPos];
            }
            else
            {
                if (m_switchalgorithm == switchAlgorithm::fade)
                {
                    int readPos = static_cast<int> (m_writePos) - static_cast<int> (m_delays[cc]);
                    if (readPos < 0)
                        readPos += static_cast<int> (m_maxdelay);

                    int readPosNew = static_cast<int> (m_writePos) - static_cast<int> (m_newdelays[cc]);
                    if (readPosNew < 0)
                        readPosNew += static_cast<int> (m_maxdelay);

                    float gain;
                    gain = static_cast<float>(m_switchCounter[cc])/m_switchTime;
                    out = gain * bufferPtr[cc][readPos] + (1.f - gain) * bufferPtr[cc][readPosNew] ;
                    --m_switchCounter[cc];
                    if (m_switchCounter[cc] == 0)
                    {
                        m_delays[cc] = static_cast<double> (m_newdelays[cc]);
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
                else // tape
                {
                    m_delays[cc] += m_fadeInc[cc];
                    m_switchCounter[cc]--;
                    if (m_switchCounter[cc] == 0)
                    {
                        m_switchState[cc] = switchState::normal;
                        m_delays[cc] = static_cast<double> (static_cast<int> (m_delays[cc] + 0.5));
                    }
                    double readPosFrac = static_cast<double> (m_writePos) - m_delays[cc];
                    if (readPosFrac < 0.0)
                        readPosFrac += static_cast<double> (m_maxdelay);

                    int interp_x1 = static_cast<int>(readPosFrac);
                    double frac = readPosFrac - interp_x1;

                    int interp_x0 = interp_x1-1;
                    if (interp_x0 < 0)
                        interp_x0 += static_cast<int> (m_maxdelay);
                    
                    int interp_x2 = interp_x1 + 1;
                    if (interp_x2 >= m_maxdelay)
                        interp_x2 -= static_cast<int> (m_maxdelay);

                    int interp_x3 = interp_x1 + 2;
                    if (interp_x3 >= m_maxdelay)
                        interp_x3 -= static_cast<int> (m_maxdelay);

//                    double L1 = ()/((0-1)*(0-2)*(0-3)) // just for the history to explain the 6.0 and 2.0
//                    double L2 = ()/((1-0)*(1-2)*(1-3))
//                    double L3 = ()/((2-0)*(2-1)*(2-3))
//                    double L4 = ()/((3-0)*(3-1)*(3-2))

                    double L1 = - ((frac)*(frac-1.0)*(frac-2.0))/6.0;
                    double L2 = ((frac+1.0)*(frac-1.0)*(frac-2.0))/2.0;
                    double L3 = - ((frac+1.0)*(frac)*(frac-2.0))/2.0;
                    double L4 = ((frac+1.0)*(frac)*(frac-1.0))/6.0;

                    out = L1*bufferPtr[cc][interp_x0] + L2*bufferPtr[cc][interp_x1] 
                            + L3*bufferPtr[cc][interp_x2] + L4*bufferPtr[cc][interp_x3];
                }

            }
            out = m_lowpass[cc].processOneSample(out);
            out = m_highpass[cc].processOneSample(out);
            dataPtr[cc][kk] *= (1.f- m_drywet);
            dataPtr[cc][kk] += m_drywet*out;
            m_oldOut[cc] = out;
        }

        m_writePos++;
        if (m_writePos == m_maxdelay)
            m_writePos = 0;
    }

    return 0;
}

void jade::BasicDelayEffect::setLowpassFrequency(float fcut)
{
    for (size_t kk = 0; kk < m_nrOfChns; kk++)
    {
        m_lowpass[kk].setCutoff(fcut);
    }

}

void jade::BasicDelayEffect::setHighpassFrequency(float fcut)
{
    for (size_t kk = 0; kk < m_nrOfChns; kk++)
    {
        m_highpass[kk].setCutoff(fcut);
    }
}

void jade::BasicDelayEffect::reset()
{
    m_buffer.clear();
    for (size_t kk = 0; kk < m_nrOfChns; kk++)
    {
        m_oldOut[kk] = 0.f;
        m_lowpass[kk].reset();
        m_highpass[kk].reset();
    }
}

void jade::BasicDelayEffect::changeBufferSize()
{
    m_buffer.setSize (static_cast<int>(m_nrOfChns), static_cast<int>(m_maxdelay));
    m_buffer.clear();
    m_delays.resize(m_nrOfChns);
    m_newdelays.resize(m_nrOfChns);
    m_futuredelays.resize(m_nrOfChns);
    m_switchCounter.resize(m_nrOfChns);
    m_switchState.resize(m_nrOfChns);
    m_fadeInc.resize(m_nrOfChns);

    m_feedback.resize(m_nrOfChns);
    m_Crossfeedback.resize(m_nrOfChns);
    m_oldOut.resize(m_nrOfChns);
    m_lowpass.resize(m_nrOfChns);
    m_highpass.resize(m_nrOfChns);

    for (auto cc = 0; cc < m_nrOfChns; ++cc)
    {
        m_delays[cc] = 0.0;
        m_switchCounter[cc] = 0;
        m_newdelays[cc] = 0;
        m_futuredelays[cc] = 0;
        m_fadeInc[cc] = 0.0;
        m_switchState[cc] = switchState::normal;

        m_feedback[cc] = 0.f;
        m_Crossfeedback[cc] = 0.f;
        m_oldOut[cc] = 0.f;
    }
}

void jade::BasicDelayEffect::switchalgorithmChanged()
{
    for (size_t cc = 0; cc < m_nrOfChns; ++cc)
    {
        if (m_switchState[cc] == switchState::changeTime)
            m_delays[cc] = static_cast<double> (m_newdelays[cc]);

        if (m_switchState[cc] == switchState::futureValueSet)
            m_delays[cc] = static_cast<double> (m_futuredelays[cc]);

        m_switchCounter[cc] = 0;
        m_newdelays[cc] = 0;
        m_futuredelays[cc] = 0;
        m_switchState[cc] = switchState::normal;
    }


}
