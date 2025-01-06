#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"	
#include "PluginSettings.h"

#include "BasicDelayEffect.h"
#include "IRDisplay.h"

#include "LavaLookAndFeelLeft.h"
#include "LavaLookAndFeelRight.h"
#include "LavaLookAndFeelLinkLeft.h"
#include "LavaLookAndFeelLinkRight.h"

#include "MouseButtonDetectOverlay.h"

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

const struct
{
	const std::string ID = "SwitchTime_msID";
	const std::string name = "SwitchTime_ms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 500.f;
	const float defaultValue = 20.f;
}g_paramSwitchTime_ms;

const struct
{
	const std::string ID = "DryWetID";
	const std::string name = "DryWet";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 1.f;
	const float defaultValue = 0.5f;
}g_paramDryWet;

const struct
{
	const std::string ID = "LowpassLeftID";
	const std::string name = "LowpassLeft";
	const std::string unitName = " Hz";
	const float minValue = logf(100.f);
	const float maxValue = logf(15000.f);
	const float defaultValue = logf(15000.f);
}g_paramLowpassLeft;

const struct
{
	const std::string ID = "LowpassRightID";
	const std::string name = "LowpassRight";
	const std::string unitName = " Hz";
	const float minValue = logf(100.f);
	const float maxValue = logf(15000.f);
	const float defaultValue = logf(15000.f);
}g_paramLowpassRight;

const struct
{
	const std::string ID = "HighpassLeftID";
	const std::string name = "HighpassLeft";
	const std::string unitName = " Hz";
	const float minValue = logf(10.f);
	const float maxValue = logf(5000.f);
	const float defaultValue = logf(10.f);
}g_paramHighpassLeft;

const struct
{
	const std::string ID = "HighpassRightID";
	const std::string name = "HighpassRight";
	const std::string unitName = " Hz";
	const float minValue = logf(10.f);
	const float maxValue = logf(5000.f);
	const float defaultValue = logf(10.f);
}g_paramHighpassRight;

const struct
{
	const std::string ID = "NumeratorLeftID";
	const std::string name = "NumeratorLeft";
	const std::string unitName = "";
	const int minValue = 1;
	const int maxValue = 64;
	const int defaultValue = 1;
}g_paramNumeratorLeft;

const struct
{
	const std::string ID = "DenominatorLeftID";
	const std::string name = "DenominatorLeft";
	const std::string unitName = "";
	const int minValue = 1;
	const int maxValue = 64;
	const int defaultValue = 16;
}g_paramDenominatorLeft;

const struct
{
	const std::string ID = "NumeratorRightID";
	const std::string name = "NumeratorRight";
	const std::string unitName = "";
	const int minValue = 1;
	const int maxValue = 64;
	const int defaultValue = 1;
}g_paramNumeratorRight;

const struct
{
	const std::string ID = "DenominatorRightID";
	const std::string name = "DenominatorRight";
	const std::string unitName = "";
	const int minValue = 1;
	const int maxValue = 64;
	const int defaultValue = 16;
}g_paramDenominatorRight;


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
	float getBpm(){return m_oldBpm;};

private:
	float m_fs = 44100.f;
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

	jade::AudioProcessParameter<float> m_paramSwitchTime;
	float m_SwitchTime = g_paramSwitchTime_ms.defaultValue;

    jade::AudioProcessParameter<float> m_paramDryWet;
	float m_DryWet = g_paramDryWet.defaultValue;

    jade::AudioProcessParameter<float> m_paramLowpassLeft;
	float m_LowpassLeft = g_paramLowpassLeft.defaultValue;

    jade::AudioProcessParameter<float> m_paramLowpassRight;
	float m_LowpassRight = g_paramLowpassRight.defaultValue;

    jade::AudioProcessParameter<float> m_paramHighpassLeft;
	float m_HighpassLeft = g_paramHighpassLeft.defaultValue;

    jade::AudioProcessParameter<float> m_paramHighpassRight;
	float m_HighpassRight = g_paramHighpassRight.defaultValue;

	juce::AudioPlayHead* m_playhead;

    jade::AudioProcessParameter<int> m_paramNumeratorLeft;
	int m_NumeratorLeft = 1;
    jade::AudioProcessParameter<int> m_paramDenominatorLeft;
	int m_DenominatorLeft = 2;

    jade::AudioProcessParameter<int> m_paramNumeratorRight;
	int m_NumeratorRight = 1;
    jade::AudioProcessParameter<int> m_paramDenominatorRight;
	int m_DenominatorRight = 2;

	float m_oldBpm;

};

class StereoDelayerGUI : public juce::Component, public Timer
{
public:
	StereoDelayerGUI(StereoDelayerAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
	~StereoDelayerGUI();
	void paint(juce::Graphics& g) override;
	void resized() override;
	void timerCallback() override;

private:
	StereoDelayerAudioProcessor& m_processor;
    juce::AudioProcessorValueTreeState& m_apvts; 

	juce::Slider m_DelayLeft_msSlider;
	juce::Slider m_DelayRight_msSlider;
	MouseButtonDetectOverlay m_Delay_msOverlay{m_DelayLeft_msSlider, m_DelayRight_msSlider};
	juce::Slider m_FeedbackLeftSlider;
	juce::Slider m_FeedbackRightSlider;
	MouseButtonDetectOverlay m_FeedbackOverlay{m_FeedbackLeftSlider, m_FeedbackRightSlider};
	juce::Slider m_CrossFeedbackLeftSlider;
	juce::Slider m_CrossFeedbackRightSlider;
	MouseButtonDetectOverlay m_CrossFeedbackOverlay{m_CrossFeedbackLeftSlider, m_CrossFeedbackRightSlider};
	juce::Slider m_SwitchTime_msSlider;
	juce::Slider m_DryWetSlider;
	juce::Slider m_LowpassLeftSlider;
	juce::Slider m_LowpassRightSlider;
	MouseButtonDetectOverlay m_LowpassOverlay{m_LowpassLeftSlider, m_LowpassRightSlider};
	juce::Slider m_HighpassLeftSlider;
	juce::Slider m_HighpassRightSlider;
	MouseButtonDetectOverlay m_HighpassOverlay{m_HighpassLeftSlider, m_HighpassRightSlider};
	juce::Slider m_NumeratorLeftSlider;
	juce::Slider m_DenominatorLeftSlider;
	juce::Slider m_NumeratorRightSlider;
	juce::Slider m_DenominatorRightSlider;
	MouseButtonDetectOverlay m_NumeratorOverlay{m_NumeratorLeftSlider, m_NumeratorRightSlider};
	MouseButtonDetectOverlay m_DenominatorOverlay{m_DenominatorLeftSlider, m_DenominatorRightSlider};

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DelayLeft_msAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DelayRight_msAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_FeedbackLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_FeedbackRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_CrossFeedbackRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_SwitchTime_msAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DryWetAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_LowpassLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_LowpassRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_HighpassLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_HighpassRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_NumeratorLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DenominatorLeftAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_NumeratorRightAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_DenominatorRightAttachment;

	juce::ToggleButton m_LinkLR;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_LinkLRAttachment;

	juce::ComboBox m_AlgoSwitchCombo;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_AlgoSwitchComboAttachment;

	IRDisplay m_IRDisplay;
	float m_oldBpm = -1.f;

	LavaLookAndFeelLeft m_lavaLookAndFeelLeft;
	LavaLookAndFeelRight m_lavaLookAndFeelRight;
	LavaLookAndFeelLinkLeft m_lavaLookAndFeelLinkLeft;
	LavaLookAndFeelLinkRight m_lavaLookAndFeelLinkRight;

	void linkButtonClicked();
};
