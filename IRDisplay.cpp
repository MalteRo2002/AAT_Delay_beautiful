#include "IRDisplay.h"

void IRDisplay::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.9f));

    // paint coordinate system and 1/16 grid
    g.setColour(juce::Colours::white.darker(0.1f)); //left channel
    float maxdelay = std::max(m_delay_msLeft, m_delay_msRight);
    float displayrange = getDisplayRange(maxdelay);

    auto r = getLocalBounds();
    auto s = getLocalBounds();
    int reducefac = 4;
    r.reduce(10*m_scaleFactor, 10*m_scaleFactor);
    g.drawRect(r, 1);
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
    g.setColour(juce::Colours::yellow.withAlpha(0.7f)); //left channel

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

    paintFunctional(g);


}

void IRDisplay::resized()
{
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
    g.setColour(juce::Colours::blue.brighter(0.6f).withAlpha(alphaFac)); //left channel
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
        y *= m_feedbackLeft;
        alphaFac *= 0.96f;
        g.setColour(juce::Colours::blue.brighter(0.6f).withAlpha(alphaFac)); //left channel

        int ypixel = r.getY() + (middle) *(1.f-y*m_dryWet);
        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        g.fillRect(r.getX()-xlinewidthHalf+xpixel, ypixel, xlinewidth, middle_y - ypixel);
    }
    // draw delay lines right
    alphaFac = 0.98f;
    g.setColour(juce::Colours::red.brighter(0.6f).withAlpha(alphaFac)); //right channel
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
        y *= m_feedbackRight;
        alphaFac *= 0.96f;
        g.setColour(juce::Colours::red.brighter(0.6f).withAlpha(alphaFac)); //right channel

        int ypixel = r.getBottom() - (middle) *(1.f-y*m_dryWet);
        int xlinewidth = static_cast<int> (4.f*m_scaleFactor+0.5f);
        int xlinewidthHalf = static_cast<int> (2.f*m_scaleFactor+0.5f);
        //g.drawLine(r.getX() + xpixel, ypixel, r.getX() + xpixel, middle_y, 2*m_scaleFactor);
        g.fillRect(r.getX()-xlinewidthHalf+xpixel, middle_y, xlinewidth, ypixel - middle_y);
    }

    // draw cross feedback
    alphaFac = 0.9f;
    juce::Colour crossFeedbackColorFromLeft = juce::Colours::blue.brighter(0.9f).withAlpha(alphaFac);
    juce::Colour crossFeedbackColorFromRight = juce::Colours::red.brighter(0.9f).withAlpha(alphaFac);
    g.setColour(crossFeedbackColorFromLeft); //left channel

    // delay is first delayLeft_ms to the power of Crossfeedbackleft afterwords 
    // is going to be the delayRight_ms to the power of Crossfeedbackright
    delay = m_delay_msLeft;
    y = 1.f;
    x = 0.f;
    bool left = true;
    float feedback = m_crossFeedbackLeft;
    while (x< displayrange)
    {
        if (left)
        {
            delay = m_delay_msLeft;
            feedback = m_crossFeedbackLeft;
            g.setColour(crossFeedbackColorFromLeft); //left channel
        }
        else
        {
            delay = m_delay_msRight;
            feedback = m_crossFeedbackRight;
            g.setColour(crossFeedbackColorFromRight); //right channel
        }
        x += delay;
        int xpixel = x / displayrange * r.getWidth();
        y *= feedback;
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
    x = 0.f;
    left = false;
    feedback = m_crossFeedbackRight;
    while (x< displayrange)
    {
        if (left)
        {
            delay = m_delay_msLeft;
            feedback = m_crossFeedbackLeft;
            g.setColour(crossFeedbackColorFromLeft); //left channel
        }
        else
        {
            delay = m_delay_msRight;
            feedback = m_crossFeedbackRight;
            g.setColour(crossFeedbackColorFromRight); //right channel
        }
        x += delay;
        int xpixel = x / displayrange * r.getWidth();
        y *= feedback;
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
