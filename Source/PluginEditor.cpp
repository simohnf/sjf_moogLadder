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
    setLookAndFeel( &otherLookAndFeel );
    otherLookAndFeel.drawComboBoxTick = false;
    //////////////////////////////////////////////////
    // Cutoff slider and label
    //////////////////////////////////////////////////
//    addAndMakeVisible( &cutOffLabel );
//    cutOffLabel.setText( "CutOff", juce::dontSendNotification );
//    cutOffLabel.setJustificationType( juce::Justification::centred );
//    cutOffLabel.attachToComponent( &cutOffSlider, false );
    
    addAndMakeVisible( cutOffSlider );
    cutOffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "cutOff", cutOffSlider ) );
    cutOffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    cutOffSlider.setNumDecimalPlacesToDisplay(3);
    cutOffSlider.setTextValueSuffix ("Hz");
    cutOffSlider.setSliderStyle( juce::Slider::Rotary );
    cutOffSlider.setTooltip("Cutoff frequency of filter \nNB Due to the design of this filter this will not be exact!");
    cutOffSlider.sendLookAndFeelChange();
    
    //////////////////////////////////////////////////
    // Resonance slider and label
    //////////////////////////////////////////////////
//    addAndMakeVisible( &resonanceLabel );
//    resonanceLabel.setText("Resonance", juce::dontSendNotification );
//    resonanceLabel.setJustificationType( juce::Justification::centred );
//    resonanceLabel.attachToComponent( &resonanceSlider, false );
    
    addAndMakeVisible( resonanceSlider );
    resonanceSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "resonance", resonanceSlider ) );
    resonanceSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    resonanceSlider.setNumDecimalPlacesToDisplay(3);
    resonanceSlider.setTextValueSuffix ("%");
    resonanceSlider.setSliderStyle( juce::Slider::Rotary );
    resonanceSlider.setTooltip("Resonance around the cutoff frequency");
    resonanceSlider.sendLookAndFeelChange();
    
    //////////////////////////////////////////////////
    // BassBoost slider and label
    //////////////////////////////////////////////////
//    addAndMakeVisible( &bassBoostLabel );
//    bassBoostLabel.setText("Bass Boost", juce::dontSendNotification );
//    bassBoostLabel.setJustificationType( juce::Justification::centred );
//    bassBoostLabel.attachToComponent( &bassBoostSlider, false );
    
    addAndMakeVisible( bassBoostSlider );
    bassBoostSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "bassBoost", bassBoostSlider ) );
    bassBoostSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, potSize, textHeight );
    bassBoostSlider.setNumDecimalPlacesToDisplay(3);
    bassBoostSlider.setTextValueSuffix ("%");
    bassBoostSlider.setSliderStyle( juce::Slider::Rotary );
    bassBoostSlider.setTooltip("Because of the design of this filter there is naturally a loss of bass frequencies, this parameter compensates for this");
    bassBoostSlider.sendLookAndFeelChange();
    
    addAndMakeVisible( cutOffLfoToggle );
    cutOffLfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 0 ], cutOffLfoToggle) );
    cutOffLfoToggle.setButtonText( "LFO " );
    cutOffLfoToggle.setTooltip("This turns on/off the modulator for the cutoff frequency");
    cutOffLfoToggle.sendLookAndFeelChange();
    
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
    cutOffLfoType.setTooltip("This allows you to choose between different types of modulation \nsine wave, triangle wave, sample and hold random, or noise");
    cutOffLfoType.sendLookAndFeelChange();
    
    addAndMakeVisible( &cutOffLfoSyncToggle );
    cutOffLfoSyncToggle.setButtonText( "Sync" );
    cutOffLfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 7 ], cutOffLfoSyncToggle) );
    cutOffLfoSyncToggle.onClick = [ this ] { checkCutoffSyncState(); };
    cutOffLfoSyncToggle.setTooltip("This allows you to sync the modulator to the underlying tempo");
    cutOffLfoSyncToggle.sendLookAndFeelChange();
    
    addAndMakeVisible( &cutOffLfoRateSlider );
    cutOffLfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 2 ], cutOffLfoRateSlider) );
    cutOffLfoRateSlider.setTextValueSuffix( "Hz" );
    cutOffLfoRateSlider.setSliderStyle( juce::Slider::LinearBar );
    cutOffLfoRateSlider.setTooltip("This sets the rate of modulation in Hz");
    cutOffLfoRateSlider.sendLookAndFeelChange();


//    auto divNames = new juce::StringArray("eightWholeNotes", " sevenWholeNotes", " sixWholeNotes", " fiveWholeNotes", " fourWholeNotes", " threeWholeNotes", " twoWholeNotes", " sevenQuarterNotes", " dottedWholeNote", " fiveQuarterNotes", " wholeNote", " sevenEightNotes", " dottedHalfNote", " wholeNoteTriplet", " fiveEightNotes", " halfNote", " dottedQuarterNote", " halfNoteTriplet", " quarterNote", " dottedEightNote", " quarterNoteTriplet", " eightNote");
    
//    auto beatNames = new juce::StringArray("wholeNote", "halfNote", "quarterNote", "eightNote", "sixteenthNote");
    juce::StringArray beatNames { "/1", "/2", "/4", "/8", "/16" };
    juce::StringArray beatTypes { "tup", "dot", "trip", "quin", "sept" };
//    auto beatTypes = new juce::StringArray("tuplet", "dotted", "triplet", "quintuplet", "septuplet");
    
//    cutOffLfoSyncDiv.addItemList( *divNames, 1 );
//    cutOffLfoSyncDivAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 8 ], cutOffLfoSyncDiv ) );
    
    addAndMakeVisible( &cutOffLFOnBeats );
    cutOffLFOnBeatsAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 8 ], cutOffLFOnBeats ) );
    cutOffLFOnBeats.setTooltip("This sets the number of beats for the tempo synced modulation");
    cutOffLFOnBeats.sendLookAndFeelChange();
    
    cutOffLfoBeatName.addItemList( beatNames, 1 );
    cutOffLfoBeatNameAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 9 ], cutOffLfoBeatName ) );
    addAndMakeVisible( &cutOffLfoBeatName );
    cutOffLfoBeatName.setTooltip("This allows you to choose between different rhythmic division \n/1 is a bar, /2 is half a bar, /4 is a quarter note, etc");
    cutOffLfoBeatName.sendLookAndFeelChange();
    
    cutOffLFOBeatType.addItemList( beatTypes, 1 );
    cutOffLFOBeatTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 10 ], cutOffLFOBeatType ) );
    addAndMakeVisible( &cutOffLFOBeatType );
    cutOffLFOBeatType.setTooltip("This allows you to choose between different rhythmic division types \ntuplets (normal), dotted notes, triplets, quintuplets, and septuplets");
    cutOffLFOBeatType.sendLookAndFeelChange();
    
    addAndMakeVisible( cutOffLfoDepthSlider );
    cutOffLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 3 ], cutOffLfoDepthSlider) );
    cutOffLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    cutOffLfoDepthSlider.setTooltip("This sets the depth of the modulation");
    cutOffLfoDepthSlider.sendLookAndFeelChange();
    
    addAndMakeVisible( cutOffLfoOffsetSlider );
    cutOffLfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 4 ], cutOffLfoOffsetSlider) );
    cutOffLfoOffsetSlider.setSliderStyle( juce::Slider::LinearBar );
    cutOffLfoOffsetSlider.setTooltip("This sets an offset for the modulation \nIf set to 0 the parameter will modulate above and below the set value, if set to -1 all of the modulation will be below the set value, if set to +1 all of the modulation will be above the set value, etc.");
    cutOffLfoOffsetSlider.sendLookAndFeelChange();
    
    cutOffLfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 0 ]+audioProcessor.lfoParamNames[ 5 ], cutOffLfoDutySlider) );
    cutOffLfoDutySlider.setSliderStyle( juce::Slider::LinearBar );
    cutOffLfoDutySlider.setTooltip("This sets the duty cycle for the triangle wave \nAt 0.5 it will be a normal triangle wave, at 0 it will be a descending sawthooth wave starting at 1, and at 1 it will be an ascending sawtooth wave");
    cutOffLfoDutySlider.sendLookAndFeelChange();
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    addAndMakeVisible( resonanceLfoToggle );
    resonanceLfoToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 0 ], resonanceLfoToggle) );
    resonanceLfoToggle.setButtonText( "LFO " );
    resonanceLfoToggle.setTooltip("This turns on/off the modulator for the resonance parameter");
    resonanceLfoToggle.sendLookAndFeelChange();
    
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
    resonanceLfoType.setTooltip("This allows you to choose between different types of modulation \nsine wave, triangle wave, sample and hold random, or noise");
    resonanceLfoType.sendLookAndFeelChange();

    
    
    addAndMakeVisible( &resonanceLfoSyncToggle );
    resonanceLfoSyncToggle.setButtonText( "Sync" );
    resonanceLfoSyncToggleAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 7 ], resonanceLfoSyncToggle) );
    resonanceLfoSyncToggle.onClick = [ this ] { checkResonanceSyncState(); };
    resonanceLfoSyncToggle.setTooltip("This allows you to sync the modulator to the underlying tempo");
    resonanceLfoSyncToggle.sendLookAndFeelChange();
    
    addAndMakeVisible( &resonanceLfoRateSlider );
    resonanceLfoRateSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 2 ], resonanceLfoRateSlider) );
    resonanceLfoRateSlider.setTextValueSuffix( "Hz" );
    resonanceLfoRateSlider.setSliderStyle( juce::Slider::LinearBar );
    resonanceLfoRateSlider.setTooltip("This sets the rate of modulation in Hz");
    resonanceLfoRateSlider.sendLookAndFeelChange();
    
//    resonanceLfoSyncDiv.addItemList( *divNames, 1 );
//    resonanceLfoSyncDivAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 8 ], resonanceLfoSyncDiv ) );
    
    addAndMakeVisible( &resonanceLFOnBeats );
    resonanceLFOnBeatsAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 8 ], resonanceLFOnBeats ) );
    resonanceLFOnBeats.setTooltip("This sets the number of beats for the tempo synced modulation");
    resonanceLFOnBeats.sendLookAndFeelChange();
    
    resonanceLfoBeatName.addItemList( beatNames, 1 );
    resonanceLfoBeatNameAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 9 ], resonanceLfoBeatName ) );
    addAndMakeVisible( &resonanceLfoBeatName );
    resonanceLfoBeatName.setTooltip("This allows you to choose between different rhythmic division \n/1 is a bar, /2 is half a bar, /4 is a quarter note, etc");
    resonanceLfoBeatName.sendLookAndFeelChange();
    
    resonanceLFOBeatType.addItemList( beatTypes, 1 );
    resonanceLFOBeatTypeAttachment.reset( new juce::AudioProcessorValueTreeState::ComboBoxAttachment( valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 10 ], resonanceLFOBeatType ) );
    addAndMakeVisible( &resonanceLFOBeatType );
    resonanceLFOBeatType.setTooltip("This allows you to choose between different rhythmic division types \ntuplets (normal), dotted notes, triplets, quintuplets, and septuplets");
    resonanceLFOBeatType.sendLookAndFeelChange();
    
    addAndMakeVisible( resonanceLfoDepthSlider );
    resonanceLfoDepthSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 3 ], resonanceLfoDepthSlider) );
    resonanceLfoDepthSlider.setSliderStyle( juce::Slider::LinearBar );
    resonanceLfoDepthSlider.setTooltip("This sets the depth of the modulation");
    resonanceLfoDepthSlider.sendLookAndFeelChange();
    
    addAndMakeVisible(resonanceLfoOffsetSlider );
    resonanceLfoOffsetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 4 ], resonanceLfoOffsetSlider) );
    resonanceLfoOffsetSlider.setSliderStyle( juce::Slider::LinearBar );
    resonanceLfoOffsetSlider.setTooltip("This sets an offset for the modulation \nIf set to 0 the parameter will modulate above and below the set value, if set to -1 all of the modulation will be below the set value, if set to +1 all of the modulation will be above the set value, etc.");
    resonanceLfoOffsetSlider.sendLookAndFeelChange();
    
    resonanceLfoDutySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, audioProcessor.lfoNames[ 1 ]+audioProcessor.lfoParamNames[ 5 ], resonanceLfoDutySlider) );
    resonanceLfoDutySlider.setSliderStyle( juce::Slider::LinearBar );
    resonanceLfoDutySlider.setTooltip("This sets the duty cycle for the triangle wave \nAt 0.5 it will be a normal triangle wave, at 0 it will be a descending sawthooth wave starting at 1, and at 1 it will be an ascending sawtooth wave");
    resonanceLfoDutySlider.sendLookAndFeelChange();
    
    
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
//    delete divNames;
    checkCutoffSyncState();
    checkResonanceSyncState();
    
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
    
    g.drawFittedText("CutOff", cutOffSlider.getX(), cutOffSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("Resonance", resonanceSlider.getX(), resonanceSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("BassBoost", bassBoostSlider.getX(), bassBoostSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
}

void Sjf_moogLadderAudioProcessorEditor::resized()
{
    cutOffSlider.setBounds( textHeight * 0.5f, textHeight * 2, potSize, potSize );
    
    cutOffLfoToggle.setBounds( cutOffSlider.getX(), cutOffSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    cutOffLfoType.setBounds( cutOffLfoToggle.getX(), cutOffLfoToggle.getBottom(), potSize, textHeight );
    cutOffLfoSyncToggle.setBounds( cutOffLfoType.getX(), cutOffLfoType.getBottom(), potSize, textHeight );
    cutOffLfoRateSlider.setBounds( cutOffLfoType.getX(), cutOffLfoSyncToggle.getBottom(), potSize, textHeight );
//    cutOffLfoSyncDiv.setBounds( cutOffLfoRateSlider.getX(), cutOffLfoRateSlider.getY(), cutOffLfoRateSlider.getWidth(), cutOffLfoRateSlider.getHeight() );
    cutOffLFOnBeats.setBounds( cutOffLfoType.getX(), cutOffLfoSyncToggle.getBottom(), potSize/3, textHeight );
    cutOffLfoBeatName.setBounds( cutOffLFOnBeats.getRight(), cutOffLFOnBeats.getY(), potSize/3, textHeight );
    cutOffLFOBeatType.setBounds( cutOffLfoBeatName.getRight(), cutOffLfoBeatName.getY(), potSize/3, textHeight );
    
    cutOffLfoDepthSlider.setBounds( cutOffLfoRateSlider.getX(), cutOffLfoRateSlider.getBottom(), potSize, textHeight );
    cutOffLfoOffsetSlider.setBounds( cutOffLfoDepthSlider.getX(), cutOffLfoDepthSlider.getBottom(), potSize, textHeight );
    cutOffLfoDutySlider.setBounds( cutOffLfoOffsetSlider.getX(), cutOffLfoOffsetSlider.getBottom(), potSize, textHeight );
    
    resonanceSlider.setBounds( cutOffSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
    
    resonanceLfoToggle.setBounds( resonanceSlider.getX(), resonanceSlider.getBottom()+textHeight*0.5f, potSize, textHeight );
    resonanceLfoType.setBounds( resonanceLfoToggle.getX(), resonanceLfoToggle.getBottom(), potSize, textHeight );
    resonanceLfoSyncToggle.setBounds( resonanceLfoType.getX(), resonanceLfoType.getBottom(), potSize, textHeight );
    resonanceLfoRateSlider.setBounds( resonanceLfoType.getX(), resonanceLfoSyncToggle.getBottom(), potSize, textHeight );
//    resonanceLfoSyncDiv.setBounds(resonanceLfoRateSlider.getX(), resonanceLfoRateSlider.getY(), resonanceLfoRateSlider.getWidth(), resonanceLfoRateSlider.getHeight());
    resonanceLFOnBeats.setBounds( resonanceLfoType.getX(), resonanceLfoSyncToggle.getBottom(), potSize/3, textHeight );
    resonanceLfoBeatName.setBounds( resonanceLFOnBeats.getRight(), resonanceLFOnBeats.getY(), potSize/3, textHeight );
    resonanceLFOBeatType.setBounds( resonanceLfoBeatName.getRight(), resonanceLfoBeatName.getY(), potSize/3, textHeight );
    
    resonanceLfoDepthSlider.setBounds( resonanceLfoRateSlider.getX(), resonanceLfoRateSlider.getBottom(), potSize, textHeight );
    resonanceLfoOffsetSlider.setBounds( resonanceLfoDepthSlider.getX(), resonanceLfoDepthSlider.getBottom(), potSize, textHeight );
    resonanceLfoDutySlider.setBounds( resonanceLfoOffsetSlider.getX(), resonanceLfoOffsetSlider.getBottom(), potSize, textHeight );
    
    bassBoostSlider.setBounds( resonanceSlider.getRight(), cutOffSlider.getY(), potSize, potSize );
    
    tooltipsToggle.setBounds( bassBoostSlider.getX() +textHeight, HEIGHT - textHeight*1.5, potSize- textHeight*2, textHeight);
    
    tooltipLabel.setBounds( 0, HEIGHT, WIDTH, textHeight*5 );
}


void Sjf_moogLadderAudioProcessorEditor::checkResonanceSyncState()
{
    //        resonanceLfoSyncDiv.setVisible( false );
    if ( resonanceLfoSyncToggle.getToggleState() )
    {
        resonanceLFOnBeats.setVisible( true );
        resonanceLfoBeatName.setVisible( true );
        resonanceLFOBeatType.setVisible( true );
        resonanceLfoRateSlider.setVisible( false );
        //            addAndMakeVisible( &resonanceLfoSyncDiv );
    }
    else
    {
        resonanceLFOnBeats.setVisible( false );
        resonanceLfoBeatName.setVisible( false );
        resonanceLFOBeatType.setVisible( false );
        resonanceLfoRateSlider.setVisible( true );
    }
}

void Sjf_moogLadderAudioProcessorEditor::checkCutoffSyncState()
{
    //        resonanceLfoSyncDiv.setVisible( false );
    if ( cutOffLfoSyncToggle.getToggleState() )
    {
        cutOffLFOnBeats.setVisible( true );
        cutOffLfoBeatName.setVisible( true );
        cutOffLFOBeatType.setVisible( true );
        cutOffLfoRateSlider.setVisible( false );
        //            addAndMakeVisible( &resonanceLfoSyncDiv );
    }
    else
    {
        cutOffLFOnBeats.setVisible( false );
        cutOffLfoBeatName.setVisible( false );
        cutOffLFOBeatType.setVisible( false );
        cutOffLfoRateSlider.setVisible( true );
    }
}

void Sjf_moogLadderAudioProcessorEditor::timerCallback()
{
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}
