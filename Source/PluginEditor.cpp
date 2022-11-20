/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define potSize 80.0f
#define textHeight 20.0f
//==============================================================================
Sjf_moogLadderAudioProcessorEditor::Sjf_moogLadderAudioProcessorEditor (Sjf_moogLadderAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState( vts )
{
    setLookAndFeel( &otherLookAndFeel );
    
    
    addAndMakeVisible( cutOffSlider );
    cutOffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "cutOff", cutOffSlider ) );
    cutOffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    cutOffSlider.setNumDecimalPlacesToDisplay(3);
    cutOffSlider.setTextValueSuffix ("Hz");
    cutOffSlider.setSliderStyle( juce::Slider::Rotary );
    
    addAndMakeVisible( resonanceSlider );
    resonanceSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "resonance", resonanceSlider ) );
    resonanceSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    resonanceSlider.setNumDecimalPlacesToDisplay(3);
    resonanceSlider.setTextValueSuffix ("%");
    resonanceSlider.setSliderStyle( juce::Slider::Rotary );
    
    addAndMakeVisible( bassBoostSlider );
    bassBoostSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "bassBoost", bassBoostSlider ) );
    bassBoostSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    bassBoostSlider.setNumDecimalPlacesToDisplay(3);
    bassBoostSlider.setTextValueSuffix ("%");
    bassBoostSlider.setSliderStyle( juce::Slider::Rotary );
    
    
//    juce::ToggleButton cutOffLfoToggle, resonanceLFOToggle;
//    juce::ComboBox cutOffLfoType, resonanceLFOType;
//    juce::Slider cutOffLfoRateSlider, cutOffLfoDepthSlider, cutOffLfoDutySlider, resonanceLFORateSlider, resonanceLfoDepthSlider, resonanceLFODutySlider;
//
//    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffSliderAttachment, resonanceSliderAttachment, bassBoostSliderAttachment;
//
//    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > cutOffLfoToggleAttachment, resonanceLFOToggleAttachment;
//    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > cutOffLfoTypeAttachment, resonanceLFOTypeAttachment;
//    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > cutOffLfoRateSliderAttachment, cutOffLfoDepthSliderAttachment, cutOffLfoDutySliderAttachment, resonanceLFORateSliderAttachment, resonanceLfoDepthSliderAttachment, resonanceLFODutySliderAttachment;
    
    addAndMakeVisible( cutOffLfoToggle );
    cutOffLfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 0 ], cutOffLfoToggle) );
    cutOffLfoToggle.setButtonText( "LFO " );
    
    addAndMakeVisible( cutOffLfoType );
    cutOffLfoType.addItem( "Sine", 1 );
    cutOffLfoType.addItem( "Tri", 2 );
    cutOffLfoType.addItem( "Noise1", 3 );
    cutOffLfoType.addItem( "Noise2", 4 );
    cutOffLfoTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 1 ], cutOffLfoType) );
    cutOffLfoType.onChange = [ this ]
    {
        if ( cutOffLfoType.getSelectedId() == 2 )
        {
            addAndMakeVisible( cutOffLfoDutySlider );
            cutOffLfoDutySlider.setVisible( true );
        }
        else
        {
            cutOffLfoDutySlider.setVisible( false );
        }
    };
    if ( cutOffLfoType.getSelectedId() == 2 )
    {
        addAndMakeVisible( cutOffLfoDutySlider );
    }
    
    addAndMakeVisible( cutOffLfoRateSlider );
    cutOffLfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 2 ], cutOffLfoRateSlider) );
    cutOffLfoRateSlider.setTextValueSuffix( "Hz" );
    cutOffLfoRateSlider.setSliderStyle( juce::Slider::LinearBar );
    
    addAndMakeVisible( cutOffLfoDepthSlider );
    cutOffLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 3 ], cutOffLfoDepthSlider) );
//    cutOffLfoDepthSlider.setTextValueSuffix( "%" );
    cutOffLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    
    
    cutOffLfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 4 ], cutOffLfoDutySlider) );
//    cutOffLfoDutySlider.setTextValueSuffix( "%" );
    cutOffLfoDutySlider.setSliderStyle( juce::Slider::LinearBar );
    
    
    
    addAndMakeVisible( resonanceLFOToggle );
    resonanceLFOToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 0 ], resonanceLFOToggle) );
    resonanceLFOToggle.setButtonText( "LFO " );
    
    addAndMakeVisible( resonanceLFOType );
    resonanceLFOType.addItem( "Sine", 1 );
    resonanceLFOType.addItem( "Tri", 2 );
    resonanceLFOType.addItem( "Noise1", 3 );
    resonanceLFOType.addItem( "Noise2", 4 );
    resonanceLFOTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 1 ], resonanceLFOType) );
    resonanceLFOType.onChange = [ this ]
    {
        if ( resonanceLFOType.getSelectedId() == 2 )
        {
            addAndMakeVisible( resonanceLFODutySlider );
            resonanceLFODutySlider.setVisible( true );
        }
        else
        {
            resonanceLFODutySlider.setVisible( false );
        }
    };
    if ( resonanceLFOType.getSelectedId() == 2 )
    {
        addAndMakeVisible( resonanceLFODutySlider );
    }
    
    addAndMakeVisible( resonanceLFORateSlider );
    resonanceLFORateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 2 ], resonanceLFORateSlider) );
    resonanceLFORateSlider.setTextValueSuffix( "Hz" );
    resonanceLFORateSlider.setSliderStyle( juce::Slider::LinearBar );
    
    addAndMakeVisible( resonanceLfoDepthSlider );
    resonanceLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 3 ], resonanceLfoDepthSlider) );
//    resonanceLfoDepthSlider.setTextValueSuffix( "%" );
    resonanceLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    
    
    resonanceLFODutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 4 ], resonanceLFODutySlider) );
//    resonanceLFODutySlider.setTextValueSuffix( "%" );
    resonanceLFODutySlider.setSliderStyle( juce::Slider::LinearBar );
    
    
    setSize (400, 300);
}

Sjf_moogLadderAudioProcessorEditor::~Sjf_moogLadderAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
}

//==============================================================================
void Sjf_moogLadderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void Sjf_moogLadderAudioProcessorEditor::resized()
{
    cutOffSlider.setBounds( textHeight * 0.5f, textHeight * 0.5f, potSize, potSize );
    
    cutOffLfoToggle.setBounds( cutOffSlider.getX(), cutOffSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    cutOffLfoType.setBounds( cutOffLfoToggle.getX(), cutOffLfoToggle.getBottom(), potSize, textHeight );
    cutOffLfoRateSlider.setBounds( cutOffLfoType.getX(), cutOffLfoType.getBottom(), potSize, textHeight );
    cutOffLfoDepthSlider.setBounds( cutOffLfoRateSlider.getX(), cutOffLfoRateSlider.getBottom(), potSize, textHeight );
    cutOffLfoDutySlider.setBounds( cutOffLfoDepthSlider.getX(), cutOffLfoDepthSlider.getBottom(), potSize, textHeight );
    
    
    resonanceSlider.setBounds( cutOffSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
    
    resonanceLFOToggle.setBounds( resonanceSlider.getX(), resonanceSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    resonanceLFOType.setBounds( resonanceLFOToggle.getX(), resonanceLFOToggle.getBottom(), potSize, textHeight );
    resonanceLFORateSlider.setBounds( resonanceLFOType.getX(), resonanceLFOType.getBottom(), potSize, textHeight );
    resonanceLfoDepthSlider.setBounds( resonanceLFORateSlider.getX(), resonanceLFORateSlider.getBottom(), potSize, textHeight );
    resonanceLFODutySlider.setBounds( resonanceLfoDepthSlider.getX(), resonanceLfoDepthSlider.getBottom(), potSize, textHeight );
    
    
    bassBoostSlider.setBounds( resonanceSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
}
