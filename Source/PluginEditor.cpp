/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define potSize 120
#define textHeight 20

#define WIDTH potSize*3 + textHeight
#define HEIGHT potSize + textHeight*10
//==============================================================================
Sjf_moogLadderAudioProcessorEditor::Sjf_moogLadderAudioProcessorEditor (Sjf_moogLadderAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState( vts )
{
    juce::StringArray beatNames { "/1", "/2", "/4", "/8", "/16" };
    juce::StringArray beatTypes { "tup", "dot", "trip", "quin", "sept" };
    
    setLookAndFeel( &otherLookAndFeel );
    otherLookAndFeel.drawComboBoxTick = false;
    //////////////////////////////////////////////////
    // cutoff slider
    //////////////////////////////////////////////////
    addAndMakeVisible( &cutoffSlider );
    cutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "cutoff", cutoffSlider ) );
    cutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    cutoffSlider.setNumDecimalPlacesToDisplay(3);
    cutoffSlider.setTextValueSuffix ("Hz");
    cutoffSlider.setSliderStyle( juce::Slider::Rotary );
    cutoffSlider.setTooltip("cutoff frequency of filter \nNB Due to the design of this filter this will not be exact!");
    cutoffSlider.sendLookAndFeelChange();
    
    //////////////////////////////////////////////////
    // Resonance slider
    //////////////////////////////////////////////////
    
    addAndMakeVisible( &resonanceSlider );
    resonanceSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "resonance", resonanceSlider ) );
    resonanceSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    resonanceSlider.setNumDecimalPlacesToDisplay(3);
    resonanceSlider.setTextValueSuffix ("%");
    resonanceSlider.setSliderStyle( juce::Slider::Rotary );
    resonanceSlider.setTooltip("Resonance around the cutoff frequency");
    resonanceSlider.sendLookAndFeelChange();
    
    //////////////////////////////////////////////////
    // bassboost slider
    //////////////////////////////////////////////////
    
    addAndMakeVisible( &bassBoostSlider );
    bassBoostSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "bassBoost", bassBoostSlider ) );
    bassBoostSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    bassBoostSlider.setNumDecimalPlacesToDisplay(3);
    bassBoostSlider.setTextValueSuffix ("%");
    bassBoostSlider.setSliderStyle( juce::Slider::Rotary );
    bassBoostSlider.setTooltip("Because of the design of this filter there is naturally a loss of bass frequencies, this parameter compensates for this");
    bassBoostSlider.sendLookAndFeelChange();
    
    addAndMakeVisible( &cutofflfoToggle );
    cutofflfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 0 ], cutofflfoToggle) );
    cutofflfoToggle.setButtonText( "LFO" );
    cutofflfoToggle.setTooltip("This turns on/off the modulator for the cutoff frequency");
    cutofflfoToggle.sendLookAndFeelChange();
    
    
    addAndMakeVisible( &cutofflfoInterface );
    cutofflfoTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 1 ], cutofflfoInterface.m_lfoType ) );
    cutofflfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 2 ], cutofflfoInterface.m_lfoRateSlider ) );
    cutofflfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 3 ], cutofflfoInterface.m_lfoDepthSlider) );
    cutofflfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 4 ], cutofflfoInterface.m_lfoOffsetSlider) );
    cutofflfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 5 ], cutofflfoInterface.m_lfoTriangleDutySlider ) );
    cutofflfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 7 ], cutofflfoInterface.m_lfoSyncToggle) );
    cutofflfonBeatsAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 8 ], cutofflfoInterface.m_lfonBeats ) );
    cutofflfoBeatNameAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 9 ], cutofflfoInterface.m_lfoBeatName ) );
    cutofflfoBeatTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 10 ], cutofflfoInterface.m_lfoBeatType ) );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    addAndMakeVisible( resonancelfoToggle );
    resonancelfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 0 ], resonancelfoToggle) );
    resonancelfoToggle.setButtonText( "LFO" );
    resonancelfoToggle.setTooltip("This turns on/off the modulator for the resonance parameter");
    resonancelfoToggle.sendLookAndFeelChange();
    
    addAndMakeVisible( &resonancelfoInterface );
    resonancelfoTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 1 ], resonancelfoInterface.m_lfoType ) );
    resonancelfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 2 ], resonancelfoInterface.m_lfoRateSlider ) );
    resonancelfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 3 ], resonancelfoInterface.m_lfoDepthSlider) );
    resonancelfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 4 ], resonancelfoInterface.m_lfoOffsetSlider) );
    resonancelfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 5 ], resonancelfoInterface.m_lfoTriangleDutySlider ) );
    resonancelfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 7 ], resonancelfoInterface.m_lfoSyncToggle) );
    resonancelfonBeatsAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 8 ], resonancelfoInterface.m_lfonBeats ) );
    resonancelfoBeatNameAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 9 ], resonancelfoInterface.m_lfoBeatName ) );
    resonancelfoBeatTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 10 ], resonancelfoInterface.m_lfoBeatType ) );
    

    
    addAndMakeVisible( &tooltipsToggle );
    tooltipsToggle.setButtonText( "Hints" );
    tooltipsToggle.onClick = [ this ]
    {
        if ( tooltipsToggle.getToggleState() )
            setSize ( WIDTH, HEIGHT + tooltipLabel.getHeight() );
        else
            setSize (WIDTH, HEIGHT);
    };
    tooltipsToggle.setTooltip( MAIN_TOOLTIP );
    
    addAndMakeVisible( &tooltipLabel );
    tooltipLabel.setColour( juce::Label::backgroundColourId, otherLookAndFeel.backGroundColour.withAlpha( 0.85f ) );
    tooltipLabel.setTooltip( MAIN_TOOLTIP );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    startTimer( 200 );
    
    setSize ( WIDTH, HEIGHT );
}

Sjf_moogLadderAudioProcessorEditor::~Sjf_moogLadderAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
}

//==============================================================================
void Sjf_moogLadderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    juce::Rectangle<int> r { WIDTH, HEIGHT };
    sjf_makeBackground< 40 >( g, r );

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("sjf_moogLadder", 0, 0, getWidth(), textHeight, juce::Justification::centred, 1);
    
    g.drawFittedText("cutoff", cutoffSlider.getX(), cutoffSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("Resonance", resonanceSlider.getX(), resonanceSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("BassBoost", bassBoostSlider.getX(), bassBoostSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
}

void Sjf_moogLadderAudioProcessorEditor::resized()
{
    cutoffSlider.setBounds( textHeight * 0.5f, textHeight * 2, potSize, potSize );
    cutofflfoToggle.setBounds( cutoffSlider.getX(), cutoffSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    cutofflfoInterface.setBounds( cutofflfoToggle.getX(), cutofflfoToggle.getBottom(), potSize, textHeight*6 );

    
    resonanceSlider.setBounds( cutoffSlider.getRight(), cutoffSlider.getY(), potSize, potSize );
    resonancelfoToggle.setBounds( resonanceSlider.getX(), resonanceSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    resonancelfoInterface.setBounds( resonancelfoToggle.getX(), resonancelfoToggle.getBottom(), potSize, textHeight*6 );

    
    bassBoostSlider.setBounds( resonanceSlider.getRight(), cutoffSlider.getY(), potSize, potSize );
    
    tooltipsToggle.setBounds( bassBoostSlider.getX() +textHeight, HEIGHT - textHeight*1.5, potSize- textHeight*2, textHeight);
    tooltipLabel.setBounds( 0, HEIGHT, WIDTH, textHeight*5 );
}


void Sjf_moogLadderAudioProcessorEditor::timerCallback()
{
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}
