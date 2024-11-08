#include <math.h>
#include "StereoDelayer.h"

#include "PluginProcessor.h"
StereoDelayerAudio::StereoDelayerAudio(juce::AudioProcessor* processor)
:SynchronBlockProcessor(), m_processor(processor)
{
}

void StereoDelayerAudio::prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels)
{
    juce::ignoreUnused(max_samplesPerBlock,max_channels);
    int synchronblocksize;
    synchronblocksize = static_cast<int>(round(g_desired_blocksize_ms * sampleRate * 0.001)); // 0.001 to transform ms to seconds;
    if (g_forcePowerOf2)
    {
        int nextpowerof2 = int(log2(synchronblocksize))+1;
        synchronblocksize = int(pow(2,nextpowerof2));
    }
    prepareSynchronProcessing(max_channels,synchronblocksize);
    m_Latency += synchronblocksize;
    // here your code
    m_delay.setSamplerate(sampleRate);
    m_delay.setMaxDelay_s(5.1f);
    m_delay.setNrOfChns(2);
    m_delay.setDelay_s(g_paramLeftDelayms.defaultValue*0.001f,0);
    m_delay.setDelay_s(g_paramRightDelayms.defaultValue*0.001f,1);
    m_delay.setSwitchTime(sampleRate*0.1);
    m_delay.setSwitchAlgorithm(jade::BasicDelayEffect::switchAlgorithm::fade);
    m_delay.setFeedback(g_paramLeftFeedback.defaultValue,0);
    m_delay.setFeedback(g_paramRightFeedback.defaultValue,1);

}

int StereoDelayerAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages, int NrOfBlocksSinceLastProcessBlock)
{
    juce::ignoreUnused(midiMessages, NrOfBlocksSinceLastProcessBlock);
    
    m_rms = buffer.getRMSLevel(0,0,buffer.getNumSamples());

    bool somethingChanged = false;
    somethingChanged = m_LeftDelaymsParam.updateWithNotification(m_delayTimeLeft_ms);
    if (somethingChanged)
        m_delay.setDelay_s(m_delayTimeLeft_ms*0.001,0);
    
    somethingChanged = m_RightDelaymsParam.updateWithNotification(m_delayTimeRight_ms);
    if (somethingChanged)
        m_delay.setDelay_s(m_delayTimeRight_ms*0.001,1);

    somethingChanged = m_LeftFeedbackParam.updateWithNotification(m_feedbackLeft);
    if (somethingChanged)
        m_delay.setFeedback(m_feedbackLeft,0);

    somethingChanged = m_RightFeedbackParam.updateWithNotification(m_feedbackRight);
    if (somethingChanged)
        m_delay.setFeedback(m_feedbackRight,1);


    int nrofsamples = buffer.getNumSamples();

    m_delay.processSamples(buffer);
    return 0;
}

void StereoDelayerAudio::addParameter(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &paramVector)
{
    // this is just a placeholder (necessary for compiling/testing the template)
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramLeftDelayms.ID,
        g_paramLeftDelayms.name,
        NormalisableRange<float>(g_paramLeftDelayms.minValue, g_paramLeftDelayms.maxValue),
        g_paramLeftDelayms.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramLeftDelayms.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramRightDelayms.ID,
        g_paramRightDelayms.name,
        NormalisableRange<float>(g_paramRightDelayms.minValue, g_paramRightDelayms.maxValue),
        g_paramRightDelayms.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramRightDelayms.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramLeftFeedback.ID,
        g_paramLeftFeedback.name,
        NormalisableRange<float>(g_paramLeftFeedback.minValue, g_paramLeftFeedback.maxValue),
        g_paramLeftFeedback.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramLeftFeedback.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramRightFeedback.ID,
        g_paramRightFeedback.name,
        NormalisableRange<float>(g_paramRightFeedback.minValue, g_paramRightFeedback.maxValue),
        g_paramRightFeedback.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramRightFeedback.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

}

void StereoDelayerAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    juce::ignoreUnused(vts);
    m_LeftDelaymsParam.prepareParameter(vts->getRawParameterValue(g_paramLeftDelayms.ID));
    m_RightDelaymsParam.prepareParameter(vts->getRawParameterValue(g_paramRightDelayms.ID));
    m_LeftFeedbackParam.prepareParameter(vts->getRawParameterValue(g_paramLeftFeedback.ID));
    m_RightFeedbackParam.prepareParameter(vts->getRawParameterValue(g_paramRightFeedback.ID));

}


StereoDelayerGUI::StereoDelayerGUI(StereoDelayerAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
:m_processor(p) ,m_apvts(apvts)
{
    startTimerHz(30);
}

void StereoDelayerGUI::paint(juce::Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3f));

    int startx = 20;
    int starty = 20;
    int width = 30;
    int height = 150;

    int reduce = 5;
    g.setColour(juce::Colours::cornflowerblue);
    g.fillRect(startx, starty, width, height);

    g.setColour(juce::Colours::black);
    g.fillRect(startx + reduce, starty + reduce, width - 2*reduce, height-2*reduce);

    g.setColour(juce::Colours::green.brighter(0.3));

    m_rms = jlimit(-60.f,0.f,m_rms);

    int startdisplay = static_cast<int> ((height-2*reduce)*m_rms/(-60.f));
    int heightdisplay = static_cast<int> (1 + (height - 2*reduce)*(1.f - m_rms/(-60.f)));

    if (startdisplay < 0 || startdisplay > height)
    {
        startdisplay = 0;
    }
    if (heightdisplay < 0 || heightdisplay>height)
        heightdisplay = 0;



    g.fillRect(startx + reduce, starty + reduce + startdisplay  ,width - 2*reduce, heightdisplay);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    juce::String text2display = "StereoDelayer V " + juce::String(PLUGIN_VERSION_MAJOR) + "." + juce::String(PLUGIN_VERSION_MINOR) + "." + juce::String(PLUGIN_VERSION_PATCH);
    g.drawFittedText (text2display, getLocalBounds(), juce::Justification::bottomLeft, 1);

}

void StereoDelayerGUI::resized()
{
	auto r = getLocalBounds();
    
    // if you have to place several components, use scaleFactor
    //int width = r.getWidth();
	//float scaleFactor = float(width)/g_minGuiSize_x;

    // use the given canvas in r
    juce::ignoreUnused(r);
}

void StereoDelayerGUI::timerCallback()
{
    m_rms = m_processor.m_algo.getRMS();
    repaint();
}
