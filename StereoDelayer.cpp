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
    m_delay.setMaxDelay_s(4.f);
    m_delay.setNrOfChns(2);
    m_delay.setDelay_s(g_paramDelayTimeLeft.defaultValue*0.001,0);
    m_delay.setDelay_s(g_paramDelayTimeRight.defaultValue*0.001,1);
    m_delay.setSwitchTime(sampleRate*0.001);
    m_switchTime = sampleRate*1.5f;
    m_delay.setSwitchAlgorithm(jade::BasicDelayLine::switchAlgorithm::digital);
    m_counter = 0;

}

int StereoDelayerAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages, int NrOfBlocksSinceLastProcessBlock)
{
    StereoDelayerAudioProcessor* processor = dynamic_cast<StereoDelayerAudioProcessor*>(m_processor);
    
    bool somethingchanged = false;
    somethingchanged = m_delayTimeLeftParam.updateWithNotification(m_delayTimeLeft);
    if (somethingchanged)
    {
        m_delay.setDelay_s(m_delayTimeLeft*0.001,0);
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayTimeRight.ID);
        
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(m_delayTimeLeft));
        param->endChangeGesture();
    }
    somethingchanged = m_delayTimeRightParam.updateWithNotification(m_delayTimeRight);
    if (somethingchanged)
    {
        m_delay.setDelay_s(m_delayTimeRight*0.001,1);
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayTimeLeft.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(m_delayTimeRight));
        param->endChangeGesture();
    }    
    juce::ignoreUnused(midiMessages, NrOfBlocksSinceLastProcessBlock);

    int nrofsamples = buffer.getNumSamples();

    m_counter += nrofsamples;

    if (m_counter >= m_switchTime)
    {
        m_counter -= m_switchTime;
        float rnd = static_cast<float> (rand())/RAND_MAX;
        float chn = rand()%2;
        //m_delay.setDelay_s(rnd,chn);
    }

    m_delay.processSamples(buffer);
    return 0;
}

void StereoDelayerAudio::addParameter(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &paramVector)
{
    // this is just a placeholder (necessary for compiling/testing the template)
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramDelayTimeLeft.ID,
        g_paramDelayTimeLeft.name,
        NormalisableRange<float>(g_paramDelayTimeLeft.minValue, g_paramDelayTimeLeft.maxValue),
        g_paramDelayTimeLeft.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramDelayTimeLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramDelayTimeRight.ID,
        g_paramDelayTimeRight.name,
        NormalisableRange<float>(g_paramDelayTimeRight.minValue, g_paramDelayTimeRight.maxValue),
        g_paramDelayTimeRight.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramDelayTimeRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1f;  return (String(value, MaxLen) + " Hz"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterBool>(g_paramLink.ID,
        g_paramLink.name,
        g_paramLink.defaultValue,
        AudioParameterBoolAttributes().withLabel (g_paramLink.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](bool value, int MaxLen) { return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getBoolValue(); }))

                        ));
    paramVector.push_back(std::make_unique<AudioParameterChoice>(g_paramSwitchAlgo.ID,
        g_paramSwitchAlgo.name,
        g_paramSwitchAlgo.choices,
        g_paramSwitchAlgo.defaultIndex,
        AudioParameterChoiceAttributes().withLabel (g_paramSwitchAlgo.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        // .withStringFromValueFunction (std::move ([](int value, int MaxLen) { return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getIntValue(); }))
                        ));                        


}

void StereoDelayerAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    // here your code
    m_delayTimeLeftParam.prepareParameter(vts->getRawParameterValue(g_paramDelayTimeLeft.ID));
    m_delayTimeRightParam.prepareParameter(vts->getRawParameterValue(g_paramDelayTimeRight.ID));
    m_linkParam.prepareParameter(vts->getRawParameterValue(g_paramLink.ID));
    m_switchAlgoParam.prepareParameter(vts->getRawParameterValue(g_paramSwitchAlgo.ID));


}


StereoDelayerGUI::StereoDelayerGUI(StereoDelayerAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
:m_processor(p) ,m_apvts(apvts)
{
    
}

void StereoDelayerGUI::paint(juce::Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3f));

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
