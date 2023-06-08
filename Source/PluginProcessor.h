/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../sjf_audio//sjf_moogLadder.h"
#include "../sjf_audio//sjf_lfo.h"
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

    std::vector< std::string > lfoNames { "frequency_Lfo_", "resonance_Lfo_" };
    std::vector< std::string > LfoNames { "Frequency_Lfo_", "Resonance_Lfo_" };
    std::vector< std::string > lfoParamNames { "On", "Type", "Rate", "Depth", "Offset", "TriangleDutyCycle", "SahRate", "TempoSync", "nBeats", "beatName", "beatType" };
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    const static int nMoog = 2;
    const static int nLFO = 2;
    
    std::array < sjf_moogLadder, nMoog > m_moog;
    std::array < sjf_lfo, nLFO > m_lfo;
    float m_SR = 44100;
    
    juce::SmoothedValue< float > m_cutOffSmooth, m_resonanceSmooth, m_bassBoostSmooth; // used to smooth changes to main parameters
    std::array< juce::SmoothedValue< float >, 4 > m_cutOffLfoSmooth, m_resonanceLfoSmooth; // used to smooth changes to lfo values
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* cutOffParameter = nullptr;
    std::atomic<float>* resonanceParameter = nullptr;
    std::atomic<float>* bassBoostParameter = nullptr;
    std::vector < std::atomic<float>* > cutOffLFOParameters;
    std::vector < std::atomic<float>* > resonanceLFOParameters;
    
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::PositionInfo positionInfo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessor)
};
