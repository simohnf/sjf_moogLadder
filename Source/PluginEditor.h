/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_LookAndFeel.h"
//==============================================================================
/**
*/
class Sjf_moogLadderAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Sjf_moogLadderAudioProcessorEditor (Sjf_moogLadderAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~Sjf_moogLadderAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Sjf_moogLadderAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_lookAndFeel otherLookAndFeel;
    
    juce::Slider cutOffSlider, resonanceSlider, bassBoostSlider;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffSliderAttachment, resonanceSliderAttachment, bassBoostSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessorEditor) 
};
