/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_moogLadder.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_lfo.h"
//==============================================================================
/**
*/
class Sjf_moogLadderAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Sjf_moogLadderAudioProcessor();
    ~Sjf_moogLadderAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    const static int nMoog = 2;
    const static int nLFO = 2;
    
    std::array < sjf_moogLadder, nMoog > m_moog;
    std::array < sjf_lfo, nLFO > m_lfo;
    float m_SR = 44100;
    
    juce::SmoothedValue< float > m_cutOffSmooth, m_resonanceSmooth, m_bassBoostSmooth;
    
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* cutOffParameter = nullptr;
    std::atomic<float>* resonanceParameter = nullptr;
    std::atomic<float>* bassBoostParameter = nullptr;
    std::vector < std::atomic<float>* > lfoOnParameters;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessor)
};
