#include <math.h>
#include "StereoDelayer.h"
#include "PluginProcessor.h"
#include "resources/images/stone_bin.h"


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
    m_fs = static_cast<float> (sampleRate);
    m_delay.setSamplerate(m_fs);
    m_delay.setMaxDelay_s(5.1f);
    m_delay.setNrOfChns(2);
    m_delay.setDelay_s(g_paramDelayLeft_ms.defaultValue*0.001f,0);
    m_delay.setDelay_s(g_paramDelayRight_ms.defaultValue*0.001,1);
    m_delay.setSwitchTime(static_cast<int> (sampleRate*0.5));
    m_delay.setSwitchAlgorithm(jade::BasicDelayEffect::switchAlgorithm::fade);

    m_delay.setFeedback(g_paramFeedbackLeft.defaultValue,0);
    m_delay.setFeedback(g_paramFeedbackRight.defaultValue,1);
    m_delay.setCrossFeedback(g_paramCrossFeedbackLeft.defaultValue,0);
    m_delay.setCrossFeedback(g_paramCrossFeedbackRight.defaultValue,1);
    m_delay.setLowpassFrequency(300.f,0);
    //m_delay.setHighpassFrequency(300.f,0);
    m_delay.setHighpassFrequency(5000.f,1);

    m_oldBpm = -2.f;
}


int StereoDelayerAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages, int NrOfBlocksSinceLastProcessBlock)
{
    StereoDelayerAudioProcessor* processor = dynamic_cast<StereoDelayerAudioProcessor*> (m_processor);
    juce::ignoreUnused(midiMessages, NrOfBlocksSinceLastProcessBlock);
    m_playhead = m_processor->getPlayHead();
    
    auto timeinfo = m_playhead->getPosition();
    float bpm = -1.f;
    bool somethingchanged = false;

    if (timeinfo.hasValue())
    {
        auto bpmopt = timeinfo->getBpm();
        if (bpmopt.hasValue())
            bpm = *bpmopt;
    }


    
    if (bpm != m_oldBpm && bpm > 0)
    {
        m_oldBpm = bpm;
        float delay_ms = 4.f*60000.f/bpm * m_NumeratorLeft/m_DenominatorLeft;
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayLeft_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
        delay_ms = 4.f*60000.f/bpm * m_NumeratorRight/m_DenominatorRight;
        param = processor->m_parameterVTS->getParameter(g_paramDelayRight_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
    }
    somethingchanged = m_paramNumeratorLeft.updateWithNotification(m_NumeratorLeft);
    if (somethingchanged && m_oldBpm>0)
    {
        float delay_ms = 4.f*60000.f/bpm * m_NumeratorLeft/m_DenominatorLeft;
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayLeft_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
        
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramNumeratorRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_NumeratorLeft));
            param->endChangeGesture();
        }

    }

    somethingchanged = m_paramDenominatorLeft.updateWithNotification(m_DenominatorLeft);
    if (somethingchanged && m_oldBpm>0)
    {
        float delay_ms = 4.f*60000.f/bpm * m_NumeratorLeft/m_DenominatorLeft;
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayLeft_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
        
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramDenominatorRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_DenominatorLeft));
            param->endChangeGesture();
        }

    }

    somethingchanged = m_paramNumeratorRight.updateWithNotification(m_NumeratorRight);
    if (somethingchanged && m_oldBpm>0)
    {
        float delay_ms = 4.f*60000.f/bpm * m_NumeratorRight/m_DenominatorRight;
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayRight_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
        
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramNumeratorLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_NumeratorRight));
            param->endChangeGesture();
        }

    }

    somethingchanged = m_paramDenominatorRight.updateWithNotification(m_DenominatorRight);
    if (somethingchanged && m_oldBpm>0)
    {
        float delay_ms = 4.f*60000.f/bpm * m_NumeratorRight/m_DenominatorRight;
        auto param = processor->m_parameterVTS->getParameter(g_paramDelayRight_ms.ID);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(delay_ms));
        param->endChangeGesture();
        
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramDenominatorLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_DenominatorRight));
            param->endChangeGesture();
        }

    }

    somethingchanged = m_paramLinkLR.updateWithNotification(m_LinkLR);

    somethingchanged = m_paramDelayLeft.updateWithNotification(m_delayLeft);
    if (somethingchanged)
    {
        m_delay.setDelay_s(m_delayLeft*0.001f,0);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramDelayRight_ms.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_delayLeft));
            param->endChangeGesture();
        }

    }
    somethingchanged = m_paramDelayRight.updateWithNotification(m_delayRight);
    if (somethingchanged)
    {
        m_delay.setDelay_s(m_delayRight*0.001f,1);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramDelayLeft_ms.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_delayRight));
            param->endChangeGesture();
        }
           
    }
    somethingchanged = m_paramFeedbackLeft.updateWithNotification(m_FeedbackLeft);
    if (somethingchanged)
    {
        m_delay.setFeedback(m_FeedbackLeft,0);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramFeedbackRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_FeedbackLeft));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramFeedbackRight.updateWithNotification(m_FeedbackRight);
    if (somethingchanged)
    {
        m_delay.setFeedback(m_FeedbackRight,1);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramFeedbackLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_FeedbackRight));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramCrossFeedbackLeft.updateWithNotification(m_CrossFeedbackLeft);
    if (somethingchanged)
    {
        m_delay.setCrossFeedback(m_CrossFeedbackLeft,0);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramCrossFeedbackRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_CrossFeedbackLeft));
            param->endChangeGesture();
        }

    }
    somethingchanged = m_paramCrossFeedbackRight.updateWithNotification(m_CrossFeedbackRight);
    if (somethingchanged)
    {
        m_delay.setCrossFeedback(m_CrossFeedbackRight,1);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramCrossFeedbackLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(m_CrossFeedbackRight));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramSwitchAlgo.updateWithNotification(m_SwitchAlgo);
    if (somethingchanged)
    {
        jade::BasicDelayEffect::switchAlgorithm switcher = static_cast<jade::BasicDelayEffect::switchAlgorithm> (m_SwitchAlgo);
        m_delay.setSwitchAlgorithm(switcher);
    }
    somethingchanged = m_paramSwitchTime.updateWithNotification(m_SwitchTime);
    if (somethingchanged)
    {
        m_delay.setSwitchTime(m_SwitchTime*0.001f*m_fs);
    }
    somethingchanged = m_paramDryWet.updateWithNotification(m_DryWet);
    if (somethingchanged)
    {
        m_delay.setDryWet(m_DryWet);
    }

    somethingchanged = m_paramLowpassLeft.updateWithNotification(m_LowpassLeft);
    if (somethingchanged)
    {
        m_delay.setLowpassFrequency(m_LowpassLeft,0);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramLowpassRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(log(m_LowpassLeft)));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramLowpassRight.updateWithNotification(m_LowpassRight);
    if (somethingchanged)
    {
        m_delay.setLowpassFrequency(m_LowpassRight,1);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramLowpassLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(log(m_LowpassRight)));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramHighpassLeft.updateWithNotification(m_HighpassLeft);
    if (somethingchanged)
    {
        m_delay.setHighpassFrequency(m_HighpassLeft,0);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramHighpassRight.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(log(m_HighpassLeft)));
            param->endChangeGesture();
        }
    }
    somethingchanged = m_paramHighpassRight.updateWithNotification(m_HighpassRight);
    if (somethingchanged)
    {
        m_delay.setHighpassFrequency(m_HighpassRight,1);
        if (m_LinkLR)
        {
            auto param = processor->m_parameterVTS->getParameter(g_paramHighpassLeft.ID);
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(log(m_HighpassRight)));
            param->endChangeGesture();
        }
    }


    m_delay.processSamples(buffer);
    return 0;
}

void StereoDelayerAudio::addParameter(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &paramVector)
{
    // this is just a placeholder (necessary for compiling/testing the template)
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramDelayLeft_ms.ID,
        g_paramDelayLeft_ms.name,
        NormalisableRange<float>(g_paramDelayLeft_ms.minValue, g_paramDelayLeft_ms.maxValue),
        g_paramDelayLeft_ms.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramDelayLeft_ms.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10 + 0.5f) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramDelayRight_ms.ID,
        g_paramDelayRight_ms.name,
        NormalisableRange<float>(g_paramDelayRight_ms.minValue, g_paramDelayRight_ms.maxValue),
        g_paramDelayRight_ms.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramDelayRight_ms.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10 + 0.5f) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))

                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramFeedbackLeft.ID,
        g_paramFeedbackLeft.name,
        NormalisableRange<float>(g_paramFeedbackLeft.minValue, g_paramFeedbackLeft.maxValue),
        g_paramFeedbackLeft.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramFeedbackLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramFeedbackRight.ID,
        g_paramFeedbackRight.name,
        NormalisableRange<float>(g_paramFeedbackRight.minValue, g_paramFeedbackRight.maxValue),
        g_paramFeedbackRight.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramFeedbackRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100);  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramCrossFeedbackLeft.ID,
        g_paramCrossFeedbackLeft.name,
        NormalisableRange<float>(g_paramCrossFeedbackLeft.minValue, g_paramCrossFeedbackLeft.maxValue),
        g_paramCrossFeedbackLeft.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramCrossFeedbackLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramCrossFeedbackRight.ID,
        g_paramCrossFeedbackRight.name,
        NormalisableRange<float>(g_paramCrossFeedbackRight.minValue, g_paramCrossFeedbackRight.maxValue),
        g_paramCrossFeedbackRight.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramCrossFeedbackRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100);  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterBool>(g_paramLinkLR.ID,
        g_paramLinkLR.name,
        g_paramLinkLR.defaultValue,
        AudioParameterBoolAttributes()//.withLabel (g_paramLinkLR.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100);  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterChoice>(g_paramSwitchAlgo.ID,
        g_paramSwitchAlgo.name,
        g_paramSwitchAlgo.choices,
        g_paramSwitchAlgo.defaultValue,
        AudioParameterChoiceAttributes()//.withLabel (g_paramLinkLR.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100);  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramSwitchTime_ms.ID,
        g_paramSwitchTime_ms.name,
        NormalisableRange<float>(g_paramSwitchTime_ms.minValue, g_paramSwitchTime_ms.maxValue),
        g_paramSwitchTime_ms.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramSwitchTime_ms.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10 + 0.5f) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))

                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramDryWet.ID,
        g_paramDryWet.name,
        NormalisableRange<float>(g_paramDryWet.minValue, g_paramDryWet.maxValue),
        g_paramDryWet.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramDryWet.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramLowpassLeft.ID,
        g_paramLowpassLeft.name,
        NormalisableRange<float>(g_paramLowpassLeft.minValue, g_paramLowpassLeft.maxValue),
        g_paramLowpassLeft.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramLowpassLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = 0.1f*int(exp(value)*10);
                                        if (value<=14500.f)  return (String(value, MaxLen)); else return String("off"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramLowpassRight.ID,
        g_paramLowpassRight.name,
        NormalisableRange<float>(g_paramLowpassRight.minValue, g_paramLowpassRight.maxValue),
        g_paramLowpassRight.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramLowpassRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = 0.1f*int(exp(value)*10);
                                        if (value<=14500.f)  return (String(value, MaxLen)); else return String("off"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramHighpassLeft.ID,
        g_paramHighpassLeft.name,
        NormalisableRange<float>(g_paramHighpassLeft.minValue, g_paramHighpassLeft.maxValue),
        g_paramHighpassLeft.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramHighpassLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = 0.1f*int(exp(value)*10);
                                        if (value>=12.f)  return (String(value, MaxLen)); else return String("off"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramHighpassRight.ID,
        g_paramHighpassRight.name,
        NormalisableRange<float>(g_paramHighpassRight.minValue, g_paramHighpassRight.maxValue),
        g_paramHighpassRight.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramHighpassRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = 0.1f*int(exp(value)*10);
                                        if (value>=12.f)  return (String(value, MaxLen)); else return String("off"); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterInt>(g_paramNumeratorLeft.ID,
        g_paramNumeratorLeft.name,
        g_paramNumeratorLeft.minValue, 
        g_paramNumeratorLeft.maxValue,
        g_paramNumeratorLeft.defaultValue,
        AudioParameterIntAttributes() //.withLabel (g_paramNumeratorLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterInt>(g_paramDenominatorLeft.ID,
        g_paramDenominatorLeft.name,
        g_paramDenominatorLeft.minValue, 
        g_paramDenominatorLeft.maxValue,
        g_paramDenominatorLeft.defaultValue,
        AudioParameterIntAttributes() //.withLabel (g_paramDenominatorLeft.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterInt>(g_paramNumeratorRight.ID,
        g_paramNumeratorRight.name,
        g_paramNumeratorRight.minValue, 
        g_paramNumeratorRight.maxValue,
        g_paramNumeratorRight.defaultValue,
        AudioParameterIntAttributes() //.withLabel (g_paramNumeratorRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterInt>(g_paramDenominatorRight.ID,
        g_paramDenominatorRight.name,
        g_paramDenominatorRight.minValue, 
        g_paramDenominatorRight.maxValue,
        g_paramDenominatorRight.defaultValue,
        AudioParameterIntAttributes() //.withLabel (g_paramDenominatorRight.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100 );  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));


}

void StereoDelayerAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    juce::ignoreUnused(vts);
    m_paramDelayLeft.prepareParameter(vts->getRawParameterValue(g_paramDelayLeft_ms.ID));
    m_paramDelayRight.prepareParameter(vts->getRawParameterValue(g_paramDelayRight_ms.ID));
    m_paramFeedbackLeft.prepareParameter(vts->getRawParameterValue(g_paramFeedbackLeft.ID));
    m_paramFeedbackRight.prepareParameter(vts->getRawParameterValue(g_paramFeedbackRight.ID));
    m_paramCrossFeedbackLeft.prepareParameter(vts->getRawParameterValue(g_paramCrossFeedbackLeft.ID));
    m_paramCrossFeedbackRight.prepareParameter(vts->getRawParameterValue(g_paramCrossFeedbackRight.ID));
    m_paramLinkLR.prepareParameter(vts->getRawParameterValue(g_paramLinkLR.ID));
    m_paramSwitchAlgo.prepareParameter(vts->getRawParameterValue(g_paramSwitchAlgo.ID));
    m_paramSwitchTime.prepareParameter(vts->getRawParameterValue(g_paramSwitchTime_ms.ID));
    m_paramDryWet.prepareParameter(vts->getRawParameterValue(g_paramDryWet.ID));
    m_paramLowpassLeft.prepareParameter(vts->getRawParameterValue(g_paramLowpassLeft.ID));
    m_paramLowpassLeft.changeTransformer(jade::AudioProcessParameter<float>::transformerFunc::exptransform);
    m_paramLowpassRight.prepareParameter(vts->getRawParameterValue(g_paramLowpassRight.ID));
    m_paramLowpassRight.changeTransformer(jade::AudioProcessParameter<float>::transformerFunc::exptransform);
    m_paramHighpassLeft.prepareParameter(vts->getRawParameterValue(g_paramHighpassLeft.ID));
    m_paramHighpassLeft.changeTransformer(jade::AudioProcessParameter<float>::transformerFunc::exptransform);
    m_paramHighpassRight.prepareParameter(vts->getRawParameterValue(g_paramHighpassRight.ID));
    m_paramHighpassRight.changeTransformer(jade::AudioProcessParameter<float>::transformerFunc::exptransform);
    m_paramNumeratorLeft.prepareParameter(vts->getRawParameterValue(g_paramNumeratorLeft.ID));
    m_paramDenominatorLeft.prepareParameter(vts->getRawParameterValue(g_paramDenominatorLeft.ID));
    m_paramNumeratorRight.prepareParameter(vts->getRawParameterValue(g_paramNumeratorRight.ID));
    m_paramDenominatorRight.prepareParameter(vts->getRawParameterValue(g_paramDenominatorRight.ID));


}


StereoDelayerGUI::StereoDelayerGUI(StereoDelayerAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
:m_processor(p) ,m_apvts(apvts),m_IRDisplay(apvts),m_advancedPopup(m_apvts, m_IRDisplay)
{
    setLookAndFeel(&m_lavaLookAndFeelLinkLeft);

    m_DelayLeft_msSlider.onValueChange = [this] {m_IRDisplay.setDelay_msLeft(m_DelayLeft_msSlider.getValue());};
    m_DelayLeft_msSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DelayLeft_msSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_DelayLeft_msSlider.setRange(g_paramDelayLeft_ms.minValue, g_paramDelayLeft_ms.maxValue);
    m_DelayLeft_msSlider.setTextValueSuffix(g_paramDelayLeft_ms.unitName);
    auto val = m_apvts.getRawParameterValue(g_paramDelayLeft_ms.ID);
    m_DelayLeft_msSlider.setValue(*val);
    m_DelayLeft_msAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramDelayLeft_ms.ID, m_DelayLeft_msSlider);
    m_DelayLeft_msSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_DelayLeft_msSlider);

    m_DelayRight_msSlider.onValueChange = [this] {m_IRDisplay.setDelay_msRight(m_DelayRight_msSlider.getValue());};
    m_DelayRight_msSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DelayRight_msSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_DelayRight_msSlider.setRange(g_paramDelayRight_ms.minValue, g_paramDelayRight_ms.maxValue);
    m_DelayRight_msSlider.setTextValueSuffix(g_paramDelayRight_ms.unitName);
    val = m_apvts.getRawParameterValue(g_paramDelayRight_ms.ID);
    m_DelayRight_msSlider.setValue(*val);
    m_DelayRight_msAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramDelayRight_ms.ID, m_DelayRight_msSlider);
    m_DelayRight_msSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_DelayRight_msSlider);

    m_delayLabel.setText("Delay", juce::dontSendNotification);
    m_delayLabel.setFont(juce::Font(15.0f));
    m_delayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_delayLabel);

    m_Delay_msOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_Delay_msOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_Delay_msOverlay.setParamName("Delay");
    m_Delay_msOverlay.setUnitName(" ms");
    addAndMakeVisible(m_Delay_msOverlay);

    m_FeedbackLeftSlider.onValueChange = [this] {m_IRDisplay.setFeedbackLeft(m_FeedbackLeftSlider.getValue());};
    m_FeedbackLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_FeedbackLeftSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_FeedbackLeftSlider.setRange(g_paramFeedbackLeft.minValue, g_paramFeedbackLeft.maxValue);
    m_FeedbackLeftSlider.setTextValueSuffix(g_paramFeedbackLeft.unitName);
    val = m_apvts.getRawParameterValue(g_paramFeedbackLeft.ID);
    m_FeedbackLeftSlider.setValue(*val);
    m_FeedbackLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramFeedbackLeft.ID, m_FeedbackLeftSlider);
    m_FeedbackLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_FeedbackLeftSlider);

    m_FeedbackRightSlider.onValueChange = [this] {m_IRDisplay.setFeedbackRight(m_FeedbackRightSlider.getValue());};
    m_FeedbackRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_FeedbackRightSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_FeedbackRightSlider.setRange(g_paramFeedbackRight.minValue, g_paramFeedbackRight.maxValue);
    m_FeedbackRightSlider.setTextValueSuffix(g_paramFeedbackRight.unitName);
    val = m_apvts.getRawParameterValue(g_paramFeedbackRight.ID);
    m_FeedbackRightSlider.setValue(*val);
    m_FeedbackRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramFeedbackRight.ID, m_FeedbackRightSlider);
    m_FeedbackRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_FeedbackRightSlider);

    m_FeedbackLabel.setText("Feedback", juce::dontSendNotification);
    m_FeedbackLabel.setFont(juce::Font(15.0f));
    m_FeedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_FeedbackLabel);

    m_FeedbackOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_FeedbackOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_FeedbackOverlay.setParamName("Feedback");
    m_FeedbackOverlay.setUnitName(" %");
    addAndMakeVisible(m_FeedbackOverlay);

    m_DryWetSlider.onValueChange = [this] {m_IRDisplay.setDryWet(m_DryWetSlider.getValue());};
    m_DryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_DryWetSlider.setRange(g_paramDryWet.minValue, g_paramDryWet.maxValue);
    m_DryWetSlider.setTextValueSuffix(g_paramDryWet.unitName);
    val = m_apvts.getRawParameterValue(g_paramDryWet.ID);
    m_DryWetSlider.setValue(*val);
    m_DryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramDryWet.ID, m_DryWetSlider);
    addAndMakeVisible(m_DryWetSlider);

    m_DryWetLabel.setText("Dry / Wet", juce::dontSendNotification);
    m_DryWetLabel.setFont(juce::Font(15.0f));
    m_DryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_DryWetLabel);

    m_DryWetOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DryWetOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_DryWetOverlay.setParamName("Dry / Wet");
    addAndMakeVisible(m_DryWetOverlay);

    m_LowpassLeftSlider.onValueChange = [this] {m_IRDisplay.setLowpassLeft(expf(m_LowpassLeftSlider.getValue()));};
    m_LowpassLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_LowpassLeftSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_LowpassLeftSlider.setRange(g_paramLowpassLeft.minValue, g_paramLowpassLeft.maxValue);
    m_LowpassLeftSlider.setTextValueSuffix(g_paramLowpassLeft.unitName);
    val = m_apvts.getRawParameterValue(g_paramLowpassLeft.ID);
    m_LowpassLeftSlider.setValue(*val);
    m_LowpassLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramLowpassLeft.ID, m_LowpassLeftSlider);
    m_LowpassLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_LowpassLeftSlider);

    m_LowpassRightSlider.onValueChange = [this] {m_IRDisplay.setLowpassRight(expf(m_LowpassRightSlider.getValue()));};
    m_LowpassRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_LowpassRightSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_LowpassRightSlider.setRange(g_paramLowpassRight.minValue, g_paramLowpassRight.maxValue);
    m_LowpassRightSlider.setTextValueSuffix(g_paramLowpassRight.unitName);
    val = m_apvts.getRawParameterValue(g_paramLowpassRight.ID);
    m_LowpassRightSlider.setValue(*val);
    m_LowpassRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramLowpassRight.ID, m_LowpassRightSlider);
    m_LowpassRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_LowpassRightSlider);

    m_LowpassLabel.setText("Lowpass", juce::dontSendNotification);
    m_LowpassLabel.setFont(juce::Font(15.0f));
    m_LowpassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_LowpassLabel);

    m_LowpassOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_LowpassOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_LowpassOverlay.setParamName("Lowpass");
    m_LowpassOverlay.setUnitName(" Hz");
    addAndMakeVisible(m_LowpassOverlay);

    m_HighpassLeftSlider.onValueChange = [this] {m_IRDisplay.setHighpassLeft(expf(m_HighpassLeftSlider.getValue()));};
    m_HighpassLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_HighpassLeftSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_HighpassLeftSlider.setRange(g_paramHighpassLeft.minValue, g_paramHighpassLeft.maxValue);
    m_HighpassLeftSlider.setTextValueSuffix(g_paramHighpassLeft.unitName);
    val = m_apvts.getRawParameterValue(g_paramHighpassLeft.ID);
    m_HighpassLeftSlider.setValue(*val);
    m_HighpassLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramHighpassLeft.ID, m_HighpassLeftSlider);
    m_HighpassLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_HighpassLeftSlider);

    m_HighpassRightSlider.onValueChange = [this] {m_IRDisplay.setHighpassRight(expf(m_HighpassRightSlider.getValue()));};
    m_HighpassRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_HighpassRightSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_HighpassRightSlider.setRange(g_paramHighpassRight.minValue, g_paramHighpassRight.maxValue);
    m_HighpassRightSlider.setTextValueSuffix(g_paramHighpassRight.unitName);
    val = m_apvts.getRawParameterValue(g_paramHighpassRight.ID);
    m_HighpassRightSlider.setValue(*val);
    m_HighpassRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramHighpassRight.ID, m_HighpassRightSlider);
    m_HighpassRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_HighpassRightSlider);
 
    m_HighpassLabel.setText("Highpass", juce::dontSendNotification);
    m_HighpassLabel.setFont(juce::Font(15.0f));
    m_HighpassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_HighpassLabel);

    m_HighpassOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_HighpassOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_HighpassOverlay.setParamName("Highpass");
    m_HighpassOverlay.setUnitName(" Hz");
    addAndMakeVisible(m_HighpassOverlay);

    m_NumeratorLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_NumeratorLeftSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_NumeratorLeftSlider.setRange(g_paramNumeratorLeft.minValue, g_paramNumeratorLeft.maxValue);
    m_NumeratorLeftSlider.setTextValueSuffix(g_paramNumeratorLeft.unitName);
    val = m_apvts.getRawParameterValue(g_paramNumeratorLeft.ID);
    m_NumeratorLeftSlider.setValue(*val);
    m_NumeratorLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramNumeratorLeft.ID, m_NumeratorLeftSlider);
    m_NumeratorLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_NumeratorLeftSlider);

    m_DenominatorLeftSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DenominatorLeftSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_DenominatorLeftSlider.setRange(g_paramDenominatorLeft.minValue, g_paramDenominatorLeft.maxValue);
    m_DenominatorLeftSlider.setTextValueSuffix(g_paramDenominatorLeft.unitName);
    val = m_apvts.getRawParameterValue(g_paramDenominatorLeft.ID);
    m_DenominatorLeftSlider.setValue(*val);
    m_DenominatorLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramDenominatorLeft.ID, m_DenominatorLeftSlider);
    m_DenominatorLeftSlider.setLookAndFeel(&m_lavaLookAndFeelLinkLeft);
    addAndMakeVisible(m_DenominatorLeftSlider);

    m_NumeratorRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_NumeratorRightSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_NumeratorRightSlider.setRange(g_paramNumeratorRight.minValue, g_paramNumeratorRight.maxValue);
    m_NumeratorRightSlider.setTextValueSuffix(g_paramNumeratorRight.unitName);
    val = m_apvts.getRawParameterValue(g_paramNumeratorRight.ID);
    m_NumeratorRightSlider.setValue(*val);
    m_NumeratorRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramNumeratorRight.ID, m_NumeratorRightSlider);
    m_NumeratorRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_NumeratorRightSlider);

    m_DenominatorRightSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DenominatorRightSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
    m_DenominatorRightSlider.setRange(g_paramDenominatorRight.minValue, g_paramDenominatorRight.maxValue);
    m_DenominatorRightSlider.setTextValueSuffix(g_paramDenominatorRight.unitName);
    val = m_apvts.getRawParameterValue(g_paramDenominatorRight.ID);
    m_DenominatorRightSlider.setValue(*val);
    m_DenominatorRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramDenominatorRight.ID, m_DenominatorRightSlider);
    addAndMakeVisible(m_DenominatorRightSlider);
    m_IRDisplay.setScaleFactor(m_processor.getScaleFactor());
    m_DenominatorRightSlider.setLookAndFeel(&m_lavaLookAndFeelLinkRight);
    addAndMakeVisible(m_IRDisplay);

    m_NumeratorLabel.setText("Numerator", juce::dontSendNotification);
    m_NumeratorLabel.setFont(juce::Font(15.0f));
    m_NumeratorLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_NumeratorLabel);

    m_NumeratorOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_NumeratorOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_NumeratorOverlay.setParamName("Numerator");
    addAndMakeVisible(m_NumeratorOverlay);

    m_DenominatorLabel.setText("Denominator", juce::dontSendNotification);
    m_DenominatorLabel.setFont(juce::Font(15.0f));
    m_DenominatorLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_DenominatorLabel);

    m_DenominatorOverlay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_DenominatorOverlay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_DenominatorOverlay.setParamName("Denominator");
    addAndMakeVisible(m_DenominatorOverlay);

    m_showPopupButton.setButtonText("Advanced Settings");
    m_showPopupButton.onClick = [this] { togglePopupVisibility(); };
    addAndMakeVisible(m_showPopupButton);

    m_advancedPopup.setVisible(false);
    m_advancedPopup.setButtonCallback([this] { StereoDelayerGUI::linkButtonClicked(); });
    addChildComponent(m_advancedPopup);

    startTimerHz(15);
}

StereoDelayerGUI::~StereoDelayerGUI()
{
    stopTimer();
}

void StereoDelayerGUI::paint(juce::Graphics &g)
{
    
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3f));

    auto bounds = getLocalBounds();

    if (auto image = juce::ImageFileFormat::loadFrom(stone_jpg, stone_jpg_len); image.isValid())
    {
        g.drawImageWithin(image, bounds.getX(), bounds.getY(),
                        bounds.getWidth(), bounds.getHeight(),
                        juce::RectanglePlacement::fillDestination);
    }

    g.setColour (juce::Colours::white);

    g.setFont (12.0f);
    
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
    int height = getHeight();
    int width = getWidth();

    float scaleFactor = m_processor.getScaleFactor();
// IRDisplay
    m_IRDisplay.setBounds((width - 7*width/8) / 2, 0, 7*width/8, height/2);
    m_IRDisplay.setScaleFactor(scaleFactor);
// layout
    r.removeFromTop(height/2);
    r.removeFromLeft(width/10);
    int startx = r.getX() ;
    int starty = r.getY() ;
    int knobwidth = 80 * scaleFactor;
    int knobheight = 80 * scaleFactor;
    int distance_y = 20 * scaleFactor;
    int distance_x = 10 * scaleFactor;

    m_DelayLeft_msSlider.setBounds(startx,starty,knobwidth,knobheight);
    m_DelayRight_msSlider.setBounds(startx,starty,knobwidth,knobheight);
    m_Delay_msOverlay.setBounds(startx,starty,knobwidth,knobheight);
    m_delayLabel.setBounds(startx,starty + knobheight,knobwidth,distance_y);

    m_NumeratorLeftSlider.setBounds(startx + 1*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_NumeratorRightSlider.setBounds(startx + 1*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_NumeratorOverlay.setBounds(startx + 1*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_NumeratorLabel.setBounds(startx + 1*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    m_DenominatorLeftSlider.setBounds(startx + 2*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_DenominatorRightSlider.setBounds(startx + 2*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_DenominatorOverlay.setBounds(startx + 2*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_DenominatorLabel.setBounds(startx + 2*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    m_FeedbackLeftSlider.setBounds(startx + 3*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_FeedbackRightSlider.setBounds(startx + 3*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_FeedbackOverlay.setBounds(startx + 3*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_FeedbackLabel.setBounds(startx + 3*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    m_LowpassLeftSlider.setBounds(startx + 4*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_LowpassRightSlider.setBounds(startx + 4*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_LowpassOverlay.setBounds(startx + 4*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_LowpassLabel.setBounds(startx + 4*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    m_HighpassLeftSlider.setBounds(startx + 5*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_HighpassRightSlider.setBounds(startx + 5*(knobwidth + distance_x) ,starty,knobwidth,knobheight);
    m_HighpassOverlay.setBounds(startx + 5*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_HighpassLabel.setBounds(startx + 5*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    m_DryWetSlider.setBounds(startx + 6*(knobwidth + distance_x) ,starty, knobwidth,knobheight);
    m_DryWetOverlay.setBounds(startx + 6*(knobwidth + distance_x) ,starty, knobwidth,knobheight);
    m_DryWetLabel.setBounds(startx + 6*(knobwidth + distance_x), starty + knobheight, knobwidth, distance_y);

    int buttonWidth = 80*scaleFactor;
    int buttonHeight = 20*scaleFactor;
    int margin = 5*scaleFactor;

    int buttonX = (width - buttonWidth) / 2;
    int buttonY = height - buttonHeight - margin;

    m_showPopupButton.setBounds(buttonX, buttonY, buttonWidth, buttonHeight);
    
    int popupBottom = height - 25 - buttonHeight * 0.8f;
    int popupTop = popupBottom - buttonHeight * 3 - 30;
    
    m_advancedPopup.setBounds((width - buttonWidth * 5) / 2, popupTop, buttonWidth * 5, buttonHeight * 3);
}


void StereoDelayerGUI::timerCallback()
{
    float bpm = m_processor.m_algo.getBpm();
    if (bpm != m_oldBpm)
    {
        if (bpm > 0)
        {
            m_oldBpm = bpm;
            m_IRDisplay.setBpm(bpm);
        }
        else
        {
            m_IRDisplay.setBpm(-1);
        }
    }
}

void StereoDelayerGUI::linkButtonClicked()
{
    std::vector<std::pair<Slider*, LookAndFeel*>> leftSliders = {
        {&m_DelayLeft_msSlider, &m_lavaLookAndFeelLeft},
        {&m_FeedbackLeftSlider, &m_lavaLookAndFeelLeft},
        {&m_NumeratorLeftSlider, &m_lavaLookAndFeelLeft},
        {&m_DenominatorLeftSlider, &m_lavaLookAndFeelLeft},
        {&m_HighpassLeftSlider, &m_lavaLookAndFeelLeft},
        {&m_LowpassLeftSlider, &m_lavaLookAndFeelLeft}
    };

    std::vector<std::pair<Slider*, LookAndFeel*>> rightSliders = {
        {&m_DelayRight_msSlider, &m_lavaLookAndFeelRight},
        {&m_FeedbackRightSlider, &m_lavaLookAndFeelRight},
        {&m_NumeratorRightSlider, &m_lavaLookAndFeelRight},
        {&m_DenominatorRightSlider, &m_lavaLookAndFeelRight},
        {&m_HighpassRightSlider, &m_lavaLookAndFeelRight},
        {&m_LowpassRightSlider, &m_lavaLookAndFeelRight}
    };

    LookAndFeel* linkLeftLookAndFeel = &m_lavaLookAndFeelLinkLeft;
    LookAndFeel* linkRightLookAndFeel = &m_lavaLookAndFeelLinkRight;

    bool isLinked = m_apvts.getRawParameterValue(g_paramLinkLR.ID)->load();

    for (auto& slider : leftSliders)
    {
        slider.first->setLookAndFeel(isLinked ? linkLeftLookAndFeel : slider.second);
    }

    for (auto& slider : rightSliders)
    {
        slider.first->setLookAndFeel(isLinked ? linkRightLookAndFeel : slider.second);
    }
}

void StereoDelayerGUI::mouseDown(const juce::MouseEvent& event)
{

    if (m_advancedPopup.isVisible() && !m_advancedPopup.getBounds().contains(event.getPosition()))
    {
        togglePopupVisibility();
    }

    Component::mouseDown(event);
}