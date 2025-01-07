#include "IRDisplay.h"
IRDisplay::IRDisplay(juce::AudioProcessorValueTreeState& vts)
:m_vts(vts)
{
    m_showFunctional.setButtonText("Show True impulse response");
    addAndMakeVisible(m_showFunctional);
    m_showFunctional.setToggleState(m_showFunctionalState, false);
    m_showFunctional.onClick = [this] {m_showFunctionalState = m_showFunctional.getToggleState(); repaint();};

    m_delay_msLeft = 120.f;
    m_delay_msRight = 250.f;
    m_feedbackLeft = 0.5f;
    m_feedbackRight = 0.5f;
    m_crossFeedbackLeft = 0.0f;
    m_crossFeedbackRight = 0.0f;
    m_bpm = -2.f;
    m_scaleFactor = 1.f;
    m_dryWet = 0.5f;
    m_fs = 48000.f;
    m_maxLen_s = 10.f;
    m_lenIR = m_maxLen_s*m_fs;
    m_delta.setSize(2, m_lenIR);
    m_delta.clear();
    
    m_delay.setSamplerate(m_fs);
    m_delay.setMaxDelay_s(m_maxLen_s);
    m_delay.setNrOfChns(2);
    m_delay.setDelay_s(m_delay_msLeft*0.001f,0);
    m_delay.setDelay_s(m_delay_msRight*0.001f,1);
    m_delay.setSwitchTime(static_cast<int> (m_fs*0.5));
    m_delay.setSwitchAlgorithm(jade::BasicDelayEffect::switchAlgorithm::fade);
    m_delay.setFeedback(m_feedbackLeft,0);
    m_delay.setFeedback(m_feedbackRight,1);
    m_delay.setCrossFeedback(m_crossFeedbackLeft,0);
    m_delay.setCrossFeedback(m_crossFeedbackRight,1);
    m_delay.setLowpassFrequency(15000.f,0);
    m_delay.setLowpassFrequency(15000.f,1);
    m_delay.setHighpassFrequency(2.f,0);
    m_delay.setHighpassFrequency(2.f,1);
    m_delay.setDryWet(m_dryWet);



};

void IRDisplay::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.9f));

    // paint coordinate system and 1/16 grid
    g.setColour(juce::Colours::white.darker(0.1f)); //left channel
    float maxdelay = std::max(m_delay_msLeft, m_delay_msRight);
    float displayrange = getDisplayRange(maxdelay);

    auto r = getLocalBounds();
    auto s = getLocalBounds();
    r.reduce(7*m_scaleFactor, 8*m_scaleFactor);
    //g.drawRect(r, 1);
    r.reduce(3*m_scaleFactor, 2*m_scaleFactor);
    float x = 0.f;
    float denominator = 16.f;

    if (m_bpm > 0 )
    {
        // draw bpm grid
        if (displayrange < 1500.f)
            denominator = 16.f;
        else if (displayrange < 2500.f)
            denominator = 8.f;
        else if (displayrange < 5000.f)
            denominator = 4.f;
        else if (displayrange <= 10000.f)
            denominator = 2.f;

        float bpm16el = 4.f*60000.f/m_bpm * 1.f/denominator;
        int Counter = 1;
        while (x < (displayrange - bpm16el))
        {
            x += bpm16el;
            float xpixel = x / displayrange * r.getWidth();
            g.drawLine(r.getX() + xpixel, r.getY()+ 20*m_scaleFactor, r.getX() + xpixel, r.getBottom()-20*m_scaleFactor, 0.5);
            if (Counter % 4 == 0)
                g.drawLine(r.getX() + xpixel, r.getY() + 20*m_scaleFactor, r.getX() + xpixel, r.getBottom()-20*m_scaleFactor, 1);
            
            // write text at top
            juce::String text = juce::String(Counter) + "/" + juce::String(denominator);
            g.setFont (12.0f*m_scaleFactor);
            g.drawText(text, r.getX() + xpixel - 20*m_scaleFactor, r.getY(), 40*m_scaleFactor, 20*m_scaleFactor, juce::Justification::centred);
            
            Counter++;
        }
    }
    else
    {
        g.setFont (12.0f*m_scaleFactor);
        g.drawText("No BPM Info from Host", r.getX(), r.getY(),150*m_scaleFactor,20*m_scaleFactor, juce::Justification::centred);
    }
    g.setColour(juce::Colours::grey.withAlpha(0.7f)); //left channel

    // draw grid for ms and add text below
    float ms = 0.f;
    while (ms < displayrange)
    {
        if (displayrange < 100.f)
            ms += 10.f;
        else if (displayrange < 200.f)
            ms += 25.f;
        else if (displayrange < 400.f)
            ms += 50.f;
        else if (displayrange < 800.f)
            ms += 100.f;
        else if (displayrange < 1600.f)
            ms += 250.f;
        else if (displayrange < 3200.f)
            ms += 500.f;
        else if (displayrange < 6400.f)
            ms += 1000.f;
        else 
            ms += 1000.f;

        float xpixel = ms / displayrange * r.getWidth();
        g.drawLine(r.getX() + xpixel, r.getY()+20*m_scaleFactor, r.getX() + xpixel, r.getBottom()-20*m_scaleFactor, 2.f);
        juce::String text = juce::String(ms) + " ms";
        g.setFont (12.0f*m_scaleFactor);
        g.drawText(text, r.getX() + xpixel - 20*m_scaleFactor, r.getBottom()-20*m_scaleFactor, 60*m_scaleFactor, 20*m_scaleFactor, juce::Justification::centred);
    }

    g.setColour(juce::Colours::white.darker(0.1f)); //left channel

    //r.removeFromTop(20*m_scaleFactor);
    int middle = r.getHeight()/2;
    int middle_y = r.getY() + middle;

    g.drawLine(r.getX(), middle_y, r.getRight(), middle_y, 1);

    if (m_showFunctionalState)
        paintIR(g);
    else
        paintFunctional(g);

}

void IRDisplay::resized()
{
    m_showFunctional.setBounds(getWidth()*7/8,0,getWidth()/8,8*m_scaleFactor);
}

void IRDisplay::paintFunctional(juce::Graphics &g)
{
    auto r = getLocalBounds();
    int reducefac = 4;
    r.reduce(10*m_scaleFactor, 10*m_scaleFactor);
    int middle = r.getHeight()/2;
    int middle_y = r.getY() + middle;
    float displayrange = getDisplayRange(std::max(m_delay_msLeft, m_delay_msRight));

    // draw delay lines left
    float alphaFac = 0.98f;
    g.setColour(juce::Colours::orange.withAlpha(alphaFac)); //left channel
    // first value is always at x = 0 and heigt is drywet
    float y = r.getY() + (middle) *(m_dryWet);

    g.drawLine(r.getX(), y, r.getX(), middle_y, 4*m_scaleFactor);

    // left delays are delaytime to the power of feedbackleft
    float delay = m_delay_msLeft;
    y = 1.f;
    float x = 0.f;
    while (x< displayrange)
    {
        x += delay;
        int xpixel = x / displayrange * r.getWidth();
        alphaFac *= 0.96f;
        g.setColour(juce::Colours::orange.withAlpha(alphaFac)); //left channel

        int ypixel = r.getY() + (middle) *(1.f-y*m_dryWet);
        y *= m_feedbackLeft;

        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        g.fillRect(r.getX()-xlinewidthHalf+xpixel, ypixel, xlinewidth, middle_y - ypixel);
    }
    // draw delay lines right
    alphaFac = 0.98f;
    g.setColour(juce::Colours::yellow.withAlpha(alphaFac)); //right channel
    // first value is always at x = 0 and heigt is drywet but in the direction of the bottom
    y = r.getBottom() - (middle) *(m_dryWet);
    g.drawLine(r.getX(), y, r.getX(), middle_y, 4*m_scaleFactor);

    // right delays are delaytime to the power of feedbackright
    delay = m_delay_msRight;
    y = 1.f;
    x = 0.f;
    while (x< displayrange)
    {
        x += delay;
        int xpixel = x / displayrange * r.getWidth();
        alphaFac *= 0.96f;
        g.setColour(juce::Colours::yellow.withAlpha(alphaFac)); //right channel

        int ypixel = r.getBottom() - (middle) *(1.f-y*m_dryWet);
        y *= m_feedbackRight;
        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        g.fillRect(r.getX()-xlinewidthHalf+xpixel, middle_y, xlinewidth, ypixel - middle_y);
    }

    // draw cross feedback
    alphaFac = 0.85f;
    juce::Colour crossFeedbackColorFromLeft = juce::Colours::red.brighter(2.0f).withAlpha(alphaFac);
    juce::Colour crossFeedbackColorFromRight = juce::Colours::yellow.brighter(2.0f).withAlpha(alphaFac);
    g.setColour(crossFeedbackColorFromLeft); //left channel

    // delay is first delayLeft_ms to the power of Crossfeedbackleft afterwords 
    // is going to be the delayRight_ms to the power of Crossfeedbackright
    delay = m_delay_msLeft;
    y = 1.f;
    x = m_delay_msLeft;
    bool left = true;
    float feedback = m_crossFeedbackLeft;
    while (x< displayrange)
    {
        if (left)
        {
            //delay = m_delay_msLeft;
            delay = m_delay_msRight;
            feedback = m_crossFeedbackLeft;
            g.setColour(crossFeedbackColorFromLeft); //left channel
        }
        else
        {
            delay = m_delay_msLeft;
            //delay = m_delay_msRight;
            feedback = m_crossFeedbackRight;
            g.setColour(crossFeedbackColorFromRight); //right channel
        }
        x += delay;
        y *= feedback;
        int xpixel = x / displayrange * r.getWidth();
        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        if (!left)
        {
            int ypixel = r.getY() + (middle) *(1.f-y*m_dryWet);
            g.fillRect(r.getX()-xlinewidthHalf+xpixel, ypixel, xlinewidth, middle_y - ypixel);
        }
        else
        {
            int ypixel = r.getBottom() - (middle) *(1.f-y*m_dryWet);
            g.fillRect(r.getX()-xlinewidthHalf+xpixel, middle_y, xlinewidth, ypixel - middle_y);
        }
        left = !left;
    }
    // And for the right starting impulse
    delay = m_delay_msRight;
    y = 1.f;
    x = m_delay_msRight;
    left = false;
    feedback = m_crossFeedbackRight;
    while (x< displayrange)
    {
        if (left)
        {
            delay = m_delay_msRight;
            //delay = m_delay_msLeft;
            feedback = m_crossFeedbackLeft;
            g.setColour(crossFeedbackColorFromLeft); //left channel
        }
        else
        {
            delay = m_delay_msLeft;
            //delay = m_delay_msRight;
            feedback = m_crossFeedbackRight;
            g.setColour(crossFeedbackColorFromRight); //right channel
        }
        x += delay;
        y *= feedback;
        int xpixel = x / displayrange * r.getWidth();
        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        if (!left)
        {
            int ypixel = r.getY() + (middle) *(1.f-y*m_dryWet);
            g.fillRect(r.getX()-xlinewidthHalf+xpixel, ypixel, xlinewidth, middle_y - ypixel);
        }
        else
        {
            int ypixel = r.getBottom() - (middle) *(1.f-y*m_dryWet);
            g.fillRect(r.getX()-xlinewidthHalf+xpixel, middle_y, xlinewidth, ypixel - middle_y);
        }
        left = !left;
    }

}

void IRDisplay::paintIR(juce::Graphics &g)
{
    auto r = getLocalBounds();
    int reducefac = 4;
    r.reduce(10*m_scaleFactor, 10*m_scaleFactor);
    int middle = r.getHeight()/2;
    int middle_y = r.getY() + middle;
    float displayrange = getDisplayRange(std::max(m_delay_msLeft, m_delay_msRight));

    // define delta impulse in m_delta
    int nr_of_samples = static_cast<int> (displayrange*0.001f*m_fs);
    m_delta.setSize(2, nr_of_samples);
    m_delta.clear();
    auto dataPtr = m_delta.getArrayOfWritePointers();
    dataPtr[0][0] = 1.f;
    dataPtr[1][0] = 1.f;

    // calculate impulse response
    m_delay.reset();
    m_delay.processSamples(m_delta);

    // paint impulse response
    float y = 0.f;
    float x = 0.f;
    int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
    int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
    

    for (int kk = 0; kk < m_delta.getNumSamples(); kk++)
    {
        g.setColour(juce::Colours::orange);

        x = static_cast<float> (kk) / m_fs * 1000.f;
        y = fabs(dataPtr[0][kk]);
        if (y>1.f)
            y = 1.f;
        int xpixel = x / displayrange * r.getWidth();
        int ypixel = r.getY() + middle - y * middle;
        g.fillRect(r.getX() + xpixel-xlinewidthHalf, ypixel,xlinewidth , middle_y - ypixel);
        y = fabs(dataPtr[1][kk]);
        if (y>1.f)
            y = 1.f;
        ypixel = r.getY() + middle + y * middle;
        g.setColour(juce::Colours::yellow);
        g.fillRect(r.getX() + xpixel-xlinewidthHalf, middle_y, xlinewidth, ypixel - middle_y);
    }

}

float IRDisplay::getDisplayRange(float maxdelay)
{
    if (maxdelay < 25.f)
        return 100.f;
    else if (maxdelay < 50.f)
        return 200.f;
    else if (maxdelay < 100.f)
        return 400.f;
    else if (maxdelay < 200.f)
        return 800.f;
    else if (maxdelay < 400.f)
        return 1600.f;
    else if (maxdelay < 800.f)
        return 3200.f;                
    else if (maxdelay < 1600.f)
        return 6400.f;                
    else
        return 10000.f;

}
