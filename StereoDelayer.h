#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"
#include "PluginSettings.h"

#include "BasicDelayLineTV.h"

class StereoDelayerAudioProcessor;

// This is how we define our parameter as globals to use it in the audio processor as well as in the editor
const struct
{
	const std::string ID = "DelayTimeLeftID";
	const std::string name = "DelayTimeLeft";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 1000.f;
	const float defaultValue = 10.f;
}g_paramDelayTimeLeft;

const struct
{
	const std::string ID = "DelayTimeRightID";
	const std::string name = "DelayTimeRight";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 1000.f;
	const float defaultValue = 10.f;
}g_paramDelayTimeRight;

// param for the RangedValueType Bool
const struct
{
	const std::string ID = "LinkID";
	const std::string name = "LinkLR";
	const std::string unitName = "";
	const bool defaultValue = false;
}g_paramLink;

// param for the RangedValueType choice
const struct
{
	const std::string ID = "SwitchAlgoID";
	const std::string name = "SwitchAlgo";
	const std::string unitName = "";
	const juce::StringArray choices = { "Digital", "Fade", "Tape" };
	const int defaultIndex = 0;
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

	jade::BasicDelayLine m_delay;
	size_t m_counter = 0;
	size_t m_switchTime = 44100;

	// parameter handling
	jade::AudioProcessParameter<float> m_delayTimeLeftParam;
	jade::AudioProcessParameter<float> m_delayTimeRightParam;
	jade::AudioProcessParameter<bool> m_linkParam;
	jade::AudioProcessParameter<int> m_switchAlgoParam;
	float m_delayTimeLeft = g_paramDelayTimeLeft.defaultValue;
	float m_delayTimeRight = g_paramDelayTimeRight.defaultValue;
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
