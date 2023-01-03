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
    
    //////////////////////////////////////////////////
    // Cutoff slider and label
    //////////////////////////////////////////////////
    addAndMakeVisible( &cutOffLabel );
    cutOffLabel.setText( "CutOff", juce::dontSendNotification );
    cutOffLabel.setJustificationType( juce::Justification::centred );
    cutOffLabel.attachToComponent( &cutOffSlider, false );
    
    addAndMakeVisible( cutOffSlider );
    cutOffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "cutOff", cutOffSlider ) );
    cutOffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    cutOffSlider.setNumDecimalPlacesToDisplay(3);
    cutOffSlider.setTextValueSuffix ("Hz");
    cutOffSlider.setSliderStyle( juce::Slider::Rotary );
    
    //////////////////////////////////////////////////
    // Resonance slider and label
    //////////////////////////////////////////////////
    addAndMakeVisible( &resonanceLabel );
    resonanceLabel.setText("Resonance", juce::dontSendNotification );
    resonanceLabel.setJustificationType( juce::Justification::centred );
    resonanceLabel.attachToComponent( &resonanceSlider, false );
    
    addAndMakeVisible( resonanceSlider );
    resonanceSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "resonance", resonanceSlider ) );
    resonanceSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    resonanceSlider.setNumDecimalPlacesToDisplay(3);
    resonanceSlider.setTextValueSuffix ("%");
    resonanceSlider.setSliderStyle( juce::Slider::Rotary );
    
    
    //////////////////////////////////////////////////
    // BassBoost slider and label
    //////////////////////////////////////////////////
    addAndMakeVisible( &bassBoostLabel );
    bassBoostLabel.setText("Bass Boost", juce::dontSendNotification );
    bassBoostLabel.setJustificationType( juce::Justification::centred );
    bassBoostLabel.attachToComponent( &bassBoostSlider, false );
    
    addAndMakeVisible( bassBoostSlider );
    bassBoostSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "bassBoost", bassBoostSlider ) );
    bassBoostSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    bassBoostSlider.setNumDecimalPlacesToDisplay(3);
    bassBoostSlider.setTextValueSuffix ("%");
    bassBoostSlider.setSliderStyle( juce::Slider::Rotary );
    
    
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
    
    addAndMakeVisible( &cutOffLfoSyncToggle );
    cutOffLfoSyncToggle.setButtonText( "Sync" );
    cutOffLfoSyncToggle.onClick = [ this ]
    {
        cutOffLfoRateSlider.setVisible( false );
        cutOffLfoSyncDiv.setVisible( false );
        if ( cutOffLfoSyncToggle.getToggleState() )
        {
            addAndMakeVisible( &cutOffLfoSyncDiv );
        }
        else
        {
            addAndMakeVisible( &cutOffLfoRateSlider );
        }
    };
    cutOffLfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 7 ], cutOffLfoSyncToggle) );
    
    if ( cutOffLfoSyncToggle.getToggleState() ) { addAndMakeVisible( &cutOffLfoSyncDiv ); }
    else { addAndMakeVisible( &cutOffLfoRateSlider ); }
    
    cutOffLfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 2 ], cutOffLfoRateSlider) );
    cutOffLfoRateSlider.setTextValueSuffix( "Hz" );
    cutOffLfoRateSlider.setSliderStyle( juce::Slider::LinearBar );
    


    auto divNames = new juce::StringArray("eightWholeNotes", " sevenWholeNotes", " sixWholeNotes", " fiveWholeNotes", " fourWholeNotes", " threeWholeNotes", " twoWholeNotes", " sevenQuarterNotes", " dottedWholeNote", " fiveQuarterNotes", " wholeNote", " sevenEightNotes", " dottedHalfNote", " wholeNoteTriplet", " fiveEightNotes", " halfNote", " dottedQuarterNote", " halfNoteTriplet", " quarterNote", " dottedEightNote", " quarterNoteTriplet", " eightNote");
    
    cutOffLfoSyncDiv.addItemList( *divNames, 1 );
    cutOffLfoSyncDivAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 8 ], cutOffLfoSyncDiv ) );
    
    addAndMakeVisible( cutOffLfoDepthSlider );
    cutOffLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 3 ], cutOffLfoDepthSlider) );
    cutOffLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    
    addAndMakeVisible( cutOffLfoOffsetSlider );
    cutOffLfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 4 ], cutOffLfoOffsetSlider) );
    cutOffLfoOffsetSlider.setSliderStyle( juce::Slider::LinearBar );

    cutOffLfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 5 ], cutOffLfoDutySlider) );
    cutOffLfoDutySlider.setSliderStyle( juce::Slider::LinearBar );
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    addAndMakeVisible( resonanceLfoToggle );
    resonanceLfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 0 ], resonanceLfoToggle) );
    resonanceLfoToggle.setButtonText( "LFO " );
    
    addAndMakeVisible( resonanceLfoType );
    resonanceLfoType.addItem( "Sine", 1 );
    resonanceLfoType.addItem( "Tri", 2 );
    resonanceLfoType.addItem( "Noise1", 3 );
    resonanceLfoType.addItem( "Noise2", 4 );
    resonanceLfoType.onChange = [ this ]
    {
        if ( resonanceLfoType.getSelectedId() == 2 )
        {
            addAndMakeVisible( resonanceLfoDutySlider );
            resonanceLfoDutySlider.setVisible( true );
        }
        else { resonanceLfoDutySlider.setVisible( false ); }
    };
    resonanceLfoTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 1 ], resonanceLfoType) );

    
    
    addAndMakeVisible( &resonanceLfoSyncToggle );
    resonanceLfoSyncToggle.setButtonText( "Sync" );
    resonanceLfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 7 ], resonanceLfoSyncToggle) );
    resonanceLfoSyncToggle.onClick = [ this ]
    {
        resonanceLfoRateSlider.setVisible( false );
        resonanceLfoSyncDiv.setVisible( false );
        if ( resonanceLfoSyncToggle.getToggleState() )
        {
            addAndMakeVisible( &resonanceLfoSyncDiv );
        }
        else
        {
            addAndMakeVisible( &resonanceLfoRateSlider );
        }
    };
    if ( resonanceLfoSyncToggle.getToggleState() ) { addAndMakeVisible( &resonanceLfoSyncDiv ); }
    else { addAndMakeVisible( &resonanceLfoRateSlider ); }
    

    resonanceLfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 2 ], resonanceLfoRateSlider) );
    resonanceLfoRateSlider.setTextValueSuffix( "Hz" );
    resonanceLfoRateSlider.setSliderStyle( juce::Slider::LinearBar );
    
    resonanceLfoSyncDiv.addItemList( *divNames, 1 );
    resonanceLfoSyncDivAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 8 ], resonanceLfoSyncDiv ) );
    
    
    addAndMakeVisible( resonanceLfoDepthSlider );
    resonanceLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 3 ], resonanceLfoDepthSlider) );
    resonanceLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    
    addAndMakeVisible(resonanceLfoOffsetSlider );
    resonanceLfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 4 ], resonanceLfoOffsetSlider) );
    resonanceLfoOffsetSlider.setSliderStyle( juce::Slider::LinearBar );
    
    resonanceLfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 5 ], resonanceLfoDutySlider) );
    resonanceLfoDutySlider.setSliderStyle( juce::Slider::LinearBar );
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    delete divNames;
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
    g.drawFittedText("sjf_moogLadder", getWidth()/2-potSize, 0, potSize*2, textHeight, juce::Justification::centred, 1);
}

void Sjf_moogLadderAudioProcessorEditor::resized()
{
    cutOffSlider.setBounds( textHeight * 0.5f, textHeight * 2, potSize, potSize );
    
    cutOffLfoToggle.setBounds( cutOffSlider.getX(), cutOffSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    cutOffLfoType.setBounds( cutOffLfoToggle.getX(), cutOffLfoToggle.getBottom(), potSize, textHeight );
    cutOffLfoSyncToggle.setBounds( cutOffLfoType.getX(), cutOffLfoType.getBottom(), potSize, textHeight );
    cutOffLfoRateSlider.setBounds( cutOffLfoType.getX(), cutOffLfoSyncToggle.getBottom(), potSize, textHeight );
    cutOffLfoSyncDiv.setBounds( cutOffLfoRateSlider.getX(), cutOffLfoRateSlider.getY(), cutOffLfoRateSlider.getWidth(), cutOffLfoRateSlider.getHeight() );
    cutOffLfoDepthSlider.setBounds( cutOffLfoRateSlider.getX(), cutOffLfoRateSlider.getBottom(), potSize, textHeight );
    cutOffLfoOffsetSlider.setBounds( cutOffLfoDepthSlider.getX(), cutOffLfoDepthSlider.getBottom(), potSize, textHeight );
    cutOffLfoDutySlider.setBounds( cutOffLfoOffsetSlider.getX(), cutOffLfoOffsetSlider.getBottom(), potSize, textHeight );
    
    resonanceSlider.setBounds( cutOffSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
    
    resonanceLfoToggle.setBounds( resonanceSlider.getX(), resonanceSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    resonanceLfoType.setBounds( resonanceLfoToggle.getX(), resonanceLfoToggle.getBottom(), potSize, textHeight );
    resonanceLfoSyncToggle.setBounds( resonanceLfoType.getX(), resonanceLfoType.getBottom(), potSize, textHeight );
    resonanceLfoRateSlider.setBounds( resonanceLfoType.getX(), resonanceLfoSyncToggle.getBottom(), potSize, textHeight );
    resonanceLfoSyncDiv.setBounds(resonanceLfoRateSlider.getX(), resonanceLfoRateSlider.getY(), resonanceLfoRateSlider.getWidth(), resonanceLfoRateSlider.getHeight());
    resonanceLfoDepthSlider.setBounds( resonanceLfoRateSlider.getX(), resonanceLfoRateSlider.getBottom(), potSize, textHeight );
    resonanceLfoOffsetSlider.setBounds( resonanceLfoDepthSlider.getX(), resonanceLfoDepthSlider.getBottom(), potSize, textHeight );
    resonanceLfoDutySlider.setBounds( resonanceLfoOffsetSlider.getX(), resonanceLfoOffsetSlider.getBottom(), potSize, textHeight );
    
    bassBoostSlider.setBounds( resonanceSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
}
