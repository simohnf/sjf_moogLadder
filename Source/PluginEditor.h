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
    void checkResonanceSyncState();
    void checkCutoffSyncState();
    void timerCallback() override;
    
    Sjf_moogLadderAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_lookAndFeel otherLookAndFeel;
    
    juce::Label tooltipLabel;
    juce::String MAIN_TOOLTIP = "sjf_moogLdder: \nemulation of the Moog Ladder Filter with lfo modulation for cut off frequency and resonance";
    
    juce::Slider cutOffSlider, resonanceSlider, bassBoostSlider;
    
    sjf_numBox cutOffLFOnBeats, resonanceLFOnBeats;
    
    juce::ToggleButton cutOffLfoToggle, resonanceLfoToggle, cutOffLfoSyncToggle, resonanceLfoSyncToggle;
    juce::ComboBox cutOffLfoType, resonanceLfoType, /*cutOffLfoSyncDiv, resonanceLfoSyncDiv,*/ cutOffLfoBeatName, resonanceLfoBeatName, cutOffLFOBeatType, resonanceLFOBeatType;
//    sjf_label cutOffLabel, resonanceLabel, bassBoostLabel;
    juce::Slider cutOffLfoRateSlider, cutOffLfoDepthSlider, cutOffLfoOffsetSlider, cutOffLfoDutySlider, resonanceLfoRateSlider, resonanceLfoDepthSlider, resonanceLfoOffsetSlider, resonanceLfoDutySlider;
    
    juce::ToggleButton tooltipsToggle;
    
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffSliderAttachment, resonanceSliderAttachment, bassBoostSliderAttachment;

    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > cutOffLfoToggleAttachment, resonanceLfoToggleAttachment, cutOffLfoSyncToggleAttachment, resonanceLfoSyncToggleAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > cutOffLfoTypeAttachment, resonanceLfoTypeAttachment, /*cutOffLfoSyncDivAttachment, resonanceLfoSyncDivAttachment*/ cutOffLfoBeatNameAttachment, resonanceLfoBeatNameAttachment, cutOffLFOBeatTypeAttachment, resonanceLFOBeatTypeAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffLfoRateSliderAttachment, cutOffLfoDepthSliderAttachment, cutOffLfoOffsetSliderAttachment, cutOffLfoDutySliderAttachment, resonanceLfoRateSliderAttachment, resonanceLfoDepthSliderAttachment, resonanceLfoOffsetSliderAttachment, resonanceLfoDutySliderAttachment, cutOffLFOnBeatsAttachment, resonanceLFOnBeatsAttachment;
    
//    juce::StringArray divNames;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_moogLadderAudioProcessorEditor) 
};
