/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../sjf_audio/sjf_LookAndFeel.h"
#include "../../sjf_audio/sjf_Label.h"
#include "../../sjf_audio/sjf_numBox.h"
#include "../../sjf_audio/sjf_lfoInterface.h"
//==============================================================================
/**
*/
class Sjf_moogLadderAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Sjf_moogLadderAudioProcessorEditor (Sjf_moogLadderAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~Sjf_moogLadderAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    
    Sjf_moogLadderAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_lookAndFeel otherLookAndFeel;
    
    juce::Label tooltipLabel;
    juce::String MAIN_TOOLTIP = "sjf_moogLdder: \nEmulation of the Moog Ladder Filter with lfo modulation for cut off frequency and resonance";
    
    juce::Slider cutoffSlider, resonanceSlider, bassBoostSlider;
    
    
    sjf_lfoInterface cutofflfoInterface, resonancelfoInterface;
    
    juce::ToggleButton cutofflfoToggle, resonancelfoToggle;
    
    juce::ToggleButton tooltipsToggle;
    
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutoffSliderAttachment, resonanceSliderAttachment, bassBoostSliderAttachment;

    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > cutofflfoToggleAttachment, resonancelfoToggleAttachment, cutofflfoSyncToggleAttachment, resonancelfoSyncToggleAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > cutofflfoTypeAttachment, resonancelfoTypeAttachment, cutofflfoBeatNameAttachment, resonancelfoBeatNameAttachment, cutofflfoBeatTypeAttachment, resonancelfoBeatTypeAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutofflfoRateSliderAttachment, cutofflfoDepthSliderAttachment, cutofflfoOffsetSliderAttachment, cutofflfoDutySliderAttachment, resonancelfoRateSliderAttachment, resonancelfoDepthSliderAttachment, resonancelfoOffsetSliderAttachment, resonancelfoDutySliderAttachment, cutofflfonBeatsAttachment, resonancelfonBeatsAttachment;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessorEditor) 
};
