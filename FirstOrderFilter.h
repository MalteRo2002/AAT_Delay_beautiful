#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI    
    #define M_PI 3.14159265358979323846
#endif

namespace jade
{

class FirstOrderLowpass
{
public:
    FirstOrderLowpass() {
        m_fcut = 0.f; m_fs = 44100.f; computeCoeffs(); reset();};
    FirstOrderLowpass(float fcut, float fsamp)
    :m_fcut(fcut),m_fs(fsamp){computeCoeffs();reset();};

    void setCutoffFrequency(float fcut){m_fcut = fcut;computeCoeffs();};
    void setSamplingrate(float fs){m_fs = fs;computeCoeffs();};

    float processOneSample(float in)
    {
        float Out;
        Out = in*m_b0 + m_b1*m_state_b - m_a1*m_state_a;
        m_state_b = in;
        m_state_a = Out;
        return Out;
    };

    void reset(){m_state_a = 0.f; m_state_b = 0.f;};
private:
    void computeCoeffs()
    {
        if (m_fcut < 20.f || m_fcut > 0.49*m_fs)
        {
            m_b0 = 1.f;
            m_b1 = 0.f;
            m_a1 = 0.f;
            return;
        }
        float w = 2.0 * m_fs;
        float Norm;
        float fCut = m_fcut * 2.0F * M_PI;
        Norm = 1.0 / (fCut + w);
        m_a1 = (w - fCut) * Norm;
        m_b0 = m_b1 = fCut * Norm;        
    };
    float m_fcut;
    float m_fs;

    float m_b0, m_b1, m_a1;
    float m_state_b;
    float m_state_a;
};

class FirstOrderHighpass
{
public:
    FirstOrderHighpass() {
        m_fcut = 0.f; m_fs = 44100.f; computeCoeffs(); reset();};
    FirstOrderHighpass(float fcut, float fsamp)
    :m_fcut(fcut),m_fs(fsamp){computeCoeffs();reset();};

    void setCutoffFrequency(float fcut){m_fcut = fcut;computeCoeffs();};
    void setSamplingrate(float fs){m_fs = fs;computeCoeffs();};

    float processOneSample(float in)
    {
        float Out;
        Out = in*m_b0 + m_b1*m_state_b - m_a1*m_state_a;
        m_state_b = in;
        m_state_a = Out;
        return Out;
    };

    void reset(){m_state_a = 0.f; m_state_b = 0.f;};
private:
    void computeCoeffs()
    {
        if (m_fcut < 20.f || m_fcut > 0.49*m_fs)
        {
            m_b0 = 1.f;
            m_b1 = 0.f;
            m_a1 = 0.f;
            return;
        }
        float w = 2.0 * m_fs;
        float Norm;
        float fCut = m_fcut * 2.0F * M_PI;
        Norm = 1.0 / (fCut + w);
        m_a1 = (w - fCut) * Norm;
        m_b0 = m_b1 = fCut * Norm;       

        m_b0 = w * Norm;
        m_b1 = -m_b0;
        m_a1 = (w - fCut) * Norm;

    };
    float m_fcut;
    float m_fs;

    float m_b0, m_b1, m_a1;
    float m_state_b;
    float m_state_a;
};


}