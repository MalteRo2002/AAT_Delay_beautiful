#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"	
#include "PluginSettings.h"

#include "BasicDelayEffect.h"

class StereoDelayerAudioProcessor;

// This is how we define our parameter as globals to use it in the audio processor as well as in the editor
const struct
{
	const std::string ID = "DelayLeft_msID";
	const std::string name = "DelayLeft_ms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 5000.f;
	const float defaultValue = 100.f;
}g_paramDelayLeft_ms;

const struct
{
	const std::string ID = "DelayRight_msID";
	const std::string name = "DelayRight_ms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 5000.f;
	const float defaultValue = 100.f;
}g_paramDelayRight_ms;

const struct
{
	const std::string ID = "FeedbackLeftID";
	const std::string name = "FeedbackLeft";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramFeedbackLeft;

const struct
{
	const std::string ID = "FeedbackRightID";
	const std::string name = "FeedbackRight";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramFeedbackRight;

const struct
{
	const std::string ID = "CrossFeedbackLeftID";
	const std::string name = "CrossFeedbackLeft";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramCrossFeedbackLeft;

const struct
{
	const std::string ID = "CrossFeedbackRightID";
	const std::string name = "CrossFeedbackRight";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramCrossFeedbackRight;

const struct
{
	const std::string ID = "LinkLRID";
	const std::string name = "LinkLR";
	const bool defaultValue = false;
}g_paramLinkLR;

const struct
{
	const std::string ID = "SwitchAlgoID";
	const std::string name = "SwitchAlgo";
	const juce::StringArray choices = {"Digital","Fade","Tape"};
	const int defaultValue = 1;
}g_paramSwitchAlgo;



class StereoDelayerAudio : public SynchronBlockProcessor
{
public:
    StereoDelayerAudio(juce::AudioProcessor* processor);
    void prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels);
    virtual int processSynchronBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages, int NrOfBlocksSinceLastProcessBlock);

    // parameter handling
  	void addParameter(std::vector < std::unique_ptr<juce::RangedAudioParameter>>& paramVector);
    void prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState>&  vts);
    
    // some necessary info for the host
    int getLatency(){return m_Latency;};

private:
	juce::AudioProcessor* m_processor;
    int m_Latency = 0;

	jade::BasicDelayEffect m_delay;
	// parameter handling
    jade::AudioProcessParameter<float> m_paramDelayLeft;
	float m_delayLeft = g_paramDelayLeft_ms.defaultValue;

	jade::AudioProcessParameter<float> m_paramDelayRight;
	float m_delayRight = g_paramDelayRight_ms.defaultValue;

    jade::AudioProcessParameter<float> m_paramFeedbackLeft;
	float m_FeedbackLeft = g_paramFeedbackLeft.defaultValue;

    jade::AudioProcessParameter<float> m_paramFeedbackRight;
	float m_FeedbackRight = g_paramFeedbackRight.defaultValue;

    jade::AudioProcessParameter<float> m_paramCrossFeedbackLeft;
	float m_CrossFeedbackLeft = g_paramCrossFeedbackLeft.defaultValue;

    jade::AudioProcessParameter<float> m_paramCrossFeedbackRight;
	float m_CrossFeedbackRight = g_paramCrossFeedbackRight.defaultValue;

    jade::AudioProcessParameter<bool> m_paramLinkLR;
	bool m_LinkLR = g_paramLinkLR.defaultValue;

    jade::AudioProcessParameter<int> m_paramSwitchAlgo;
	int m_SwitchAlgo = g_paramSwitchAlgo.defaultValue;


};

class StereoDelayerGUI : public juce::Component
{
public:
	StereoDelayerGUI(StereoDelayerAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);

	void paint(juce::Graphics& g) override;
	void resized() override;
private:
	StereoDelayerAudioProcessor& m_processor;
    juce::AudioProcessorValueTreeState& m_apvts; 

	juce::Slider m_DelayLeft_msSlider;
	juce::Slider m_DelayRight_msSlider;
	juce::Slider m_FeedbackLeftSlider;
	juce::Slider m_FeedbackRightSlider;
	juce::Slider m_CrossFeedbackLeftSlider;
	juce::Slider m_CrossFeedbackRightSlider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DelayLeft_msAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DelayRight_msAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_FeedbackLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_FeedbackRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackRightAttachment;

	juce::ToggleButton m_LinkLR;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_LinkLRAttachment;
};
