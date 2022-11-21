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
    
    juce::ToggleButton cutOffLfoToggle, resonanceLfoToggle;
    juce::ComboBox cutOffLfoType, resonanceLfoType;
    juce::Slider cutOffLfoRateSlider, cutOffLfoDepthSlider, cutOffLfoOffsetSlider, cutOffLfoDutySlider, resonanceLfoRateSlider, resonanceLfoDepthSlider, resonanceLfoOffsetSlider, resonanceLfoDutySlider;
    
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffSliderAttachment, resonanceSliderAttachment, bassBoostSliderAttachment;

    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > cutOffLfoToggleAttachment, resonanceLfoToggleAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > cutOffLfoTypeAttachment, resonanceLfoTypeAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffLfoRateSliderAttachment, cutOffLfoDepthSliderAttachment, cutOffLfoOffsetSliderAttachment, cutOffLfoDutySliderAttachment, resonanceLfoRateSliderAttachment, resonanceLfoDepthSliderAttachment, resonanceLfoOffsetSliderAttachment, resonanceLfoDutySliderAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessorEditor) 
};
