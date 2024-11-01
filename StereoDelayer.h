#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"


#include "PluginSettings.h"

#include "BasicDelayEffect.h"

class StereoDelayerAudioProcessor;

// This is how we define our parameter as globals to use it in the audio processor as well as in the editor
const struct
{
	const std::string ID = "LeftDelaymsID";
	const std::string name = "LeftDelayms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 5000.f;
	const float defaultValue = 500.f;
}g_paramLeftDelayms;

const struct
{
	const std::string ID = "RightDelaymsID";
	const std::string name = "RightDelayms";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 5000.f;
	const float defaultValue = 500.f;
}g_paramRightDelayms;

const struct
{
	const std::string ID = "LeftFeedbackID";
	const std::string name = "LeftFeedback";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramLeftFeedback;

const struct
{
	const std::string ID = "RightFeedbackID";
	const std::string name = "RightFeedback";
	const std::string unitName = " %";
	const float minValue = 0.f;
	const float maxValue = 0.999f;
	const float defaultValue = 0.f;
}g_paramRightFeedback;


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

	// parameter code
	float m_delayTimeLeft_ms = 1.f;
	float m_delayTimeRight_ms = 1.f;
	jade::AudioProcessParameter<float> m_LeftDelaymsParam;
	jade::AudioProcessParameter<float> m_RightDelaymsParam;
	float m_feedbackLeft = 0.f;
	float m_feedbackRight = 0.f;
	jade::AudioProcessParameter<float> m_LeftFeedbackParam;
	jade::AudioProcessParameter<float> m_RightFeedbackParam;



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

};
