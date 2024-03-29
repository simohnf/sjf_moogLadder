/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_moogLadderAudioProcessor::Sjf_moogLadderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, parameters( *this, nullptr, juce::Identifier("sjf_moogLadder"), createParameterLayout() )
{
    
    
    cutOffParameter = parameters.getRawParameterValue("cutoff");
    resonanceParameter = parameters.getRawParameterValue("resonance");
    bassBoostParameter = parameters.getRawParameterValue("bassBoost"); 
    

    for ( int i = 0; i < lfoParamNames.size(); i++ )
    {
        cutOffLFOParameters.push_back( nullptr );
        cutOffLFOParameters[ i ] = parameters.getRawParameterValue( lfoNames[ 0 ]+lfoParamNames[ i ] );
        
        resonanceLFOParameters.push_back( nullptr );
        resonanceLFOParameters[ i ] = parameters.getRawParameterValue( lfoNames[ 1 ]+lfoParamNames[ i ] );
    }
    
    
}

Sjf_moogLadderAudioProcessor::~Sjf_moogLadderAudioProcessor()
{
}

//==============================================================================
const juce::String Sjf_moogLadderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Sjf_moogLadderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_moogLadderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_moogLadderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Sjf_moogLadderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Sjf_moogLadderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Sjf_moogLadderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Sjf_moogLadderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Sjf_moogLadderAudioProcessor::getProgramName (int index)
{
    return {};
}

void Sjf_moogLadderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Sjf_moogLadderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_SR = sampleRate;
    for ( int i = 0; i < m_moog.size(); i++ )
    {
        m_moog[ i ].setSampleRate( m_SR );
    }
    
    for ( int i = 0; i < m_lfo.size(); i++ )
    {
        m_lfo[ i ].setSampleRate( m_SR );
    }
    
    for ( int i = 0; i < m_cutOffLfoSmooth.size(); i++ )
    {
        m_cutOffLfoSmooth[ i ].reset( m_SR, 0.01f );
        m_resonanceLfoSmooth[ i ].reset( m_SR, 0.01f );
    }
    
    m_cutOffSmooth.reset( m_SR, 0.01f );
    m_resonanceSmooth.reset( m_SR, 0.01f );
    m_bassBoostSmooth.reset( m_SR, 0.01f );
}

void Sjf_moogLadderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Sjf_moogLadderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Sjf_moogLadderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // if there is an available playhead
    playHead = this->getPlayHead();
    if (playHead != nullptr)
    {
        positionInfo = *playHead->getPosition();
        m_lfo[ 0 ].isSyncedToTempo( *cutOffLFOParameters[ 7 ] );
        m_lfo[ 1 ].isSyncedToTempo( *resonanceLFOParameters[ 7 ] );
        for ( int i = 0; i < m_lfo.size(); i++ )
        {
            if( positionInfo.getPpqPosition() )
            {
                m_lfo[ i ].setPosition( *positionInfo.getPpqPosition() ); // set the current position
            }
            if ( positionInfo.getBpm() )
            {
                m_lfo[ i ].setBpm( *positionInfo.getBpm() ); // set bpm for internal calculations
            }
        }
    }

    m_cutOffSmooth.setTargetValue( calculateOnePoleFilterCoefficient( *cutOffParameter, m_SR ) );
    m_resonanceSmooth.setTargetValue( *resonanceParameter * 0.01f * 4.0f );
    m_bassBoostSmooth.setTargetValue( *bassBoostParameter * 0.01f );
    
    for ( int i = 0; i < m_cutOffLfoSmooth.size(); i++ )
    {
        if ( i == 0 )
        {
            if ( *cutOffLFOParameters[ 7 ] ) // check if cutoff lfo is synced
            {
                m_lfo[ 0 ].setSyncDivision( *cutOffLFOParameters [ 8 ], *cutOffLFOParameters [ 9 ], *cutOffLFOParameters [ 10 ] );
            }
            else
            {
                // for rate change we need to set target as inverse of frequency
                m_cutOffLfoSmooth[ i ].setTargetValue( 1.0f / *cutOffLFOParameters[ i + 2 ] );
            }
            if ( *resonanceLFOParameters[ 7 ] )
            {
                m_lfo[ 1 ].setSyncDivision( *resonanceLFOParameters[ 8 ], *resonanceLFOParameters[ 9 ], *resonanceLFOParameters[ 10 ] );
            }
            else
            {
                // for rate change we need to set target as inverse of frequency
                m_resonanceLfoSmooth[ i ].setTargetValue( 1.0f / *resonanceLFOParameters[ i + 2 ] );
            }
            
        }
        else if ( i == 1 )
        {
            // for max width of lfo to equal 1 we just multiply by 0.5
            m_cutOffLfoSmooth[ i ].setTargetValue( *cutOffLFOParameters[ i + 2 ] * 0.5f );
            m_resonanceLfoSmooth[ i ].setTargetValue( *resonanceLFOParameters[ i + 2 ] * 0.5f );
        }
        else
        {
            m_cutOffLfoSmooth[ i ].setTargetValue( *cutOffLFOParameters[ i + 2 ] );
            m_resonanceLfoSmooth[ i ].setTargetValue( *resonanceLFOParameters[ i + 2 ] );
        }
        
    }
    
    
    m_lfo[ 0 ].setLFOtype( *cutOffLFOParameters[ 1 ] );
    m_lfo[ 1 ].setLFOtype( *resonanceLFOParameters[ 1 ] );
    
    auto minCoef = sin( 40.0f * 3.141593 / m_SR );
    auto maxCoef = sin( 5000.0f * 3.141593 / m_SR );
    
    float coef, res, bassBoost, cutOffLfoOut, resonanceLfoOut;
    for ( int s = 0; s < buffer.getNumSamples(); s++ )
    {
        if ( !*cutOffLFOParameters[ 7 ] )
        {
            m_lfo[ 0 ].setRateChange( m_cutOffLfoSmooth[ 0 ].getNextValue() );
        }
        m_lfo[ 0 ].setOffset( m_cutOffLfoSmooth[ 2 ].getNextValue() );
        m_lfo[ 0 ].setTriangleDuty( m_cutOffLfoSmooth[ 3 ].getNextValue() );
        cutOffLfoOut = m_lfo [ 0 ].output();
        cutOffLfoOut *= m_cutOffLfoSmooth [ 1 ].getNextValue();
        coef = m_cutOffSmooth.getNextValue();
        if ( *cutOffLFOParameters[ 0 ] )
        {
            coef += ( coef * cutOffLfoOut );
        }
        coef = fmin( fmax( coef, minCoef ), maxCoef );
        if ( !*resonanceLFOParameters[ 7 ] )
        {
            m_lfo[ 1 ].setRateChange( m_resonanceLfoSmooth [ 0 ].getNextValue() );
        }
        m_lfo[ 1 ].setOffset( m_resonanceLfoSmooth [ 2 ].getNextValue() );
        m_lfo[ 1 ].setTriangleDuty( m_resonanceLfoSmooth [ 3 ].getNextValue() );
        resonanceLfoOut = m_lfo[ 1 ].output() * m_resonanceLfoSmooth [ 1 ].getNextValue();
        res = m_resonanceSmooth.getNextValue();
        if ( *resonanceLFOParameters[ 0 ] )
        {
            res += ( res * resonanceLfoOut );
        }
        
        res = fmin( fmax( res, 0.0f ), 4.0f );
        
        bassBoost = m_bassBoostSmooth.getNextValue();
        
        for ( int c = 0; c < totalNumOutputChannels; c ++ )
        {
            m_moog[ c ].setCoefficent( coef );
            m_moog[ c ].setResonanceQuick( res );
            m_moog[ c ].setBassBoostQuick( bassBoost );
            
            buffer.setSample( c, s, m_moog [ c ].filterInput( buffer.getSample( c % totalNumInputChannels, s ) ) );
        }
    }
}

//==============================================================================
bool Sjf_moogLadderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Sjf_moogLadderAudioProcessor::createEditor()
{
    return new Sjf_moogLadderAudioProcessorEditor ( *this, parameters );
}

//==============================================================================
void Sjf_moogLadderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void Sjf_moogLadderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout Sjf_moogLadderAudioProcessor::createParameterLayout()
{
    static constexpr int pIDVersionNumber = 1;
    
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    juce::NormalisableRange < float > cutOffRange( 40.0f, 5000.0f, 0.001f );
    cutOffRange.setSkewForCentre( 450.0f );
    params.add( std::make_unique<juce::AudioParameterFloat>  ( juce::ParameterID{ "cutoff", pIDVersionNumber }, "Cutoff", cutOffRange, 100.0f ) );
    params.add( std::make_unique<juce::AudioParameterFloat>  ( juce::ParameterID{ "resonance", pIDVersionNumber }, "Resonance", 0.0f, 100.0f, 100.0f ) );
    params.add( std::make_unique<juce::AudioParameterFloat>  ( juce::ParameterID{ "bassBoost", pIDVersionNumber }, "BassBoost", 0.0f, 100.0f, 100.0f ) );


    for ( int i = 0; i <  nLFO; i++ )
    {
        // on / off
        params.add( std::make_unique<juce::AudioParameterBool>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 0 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 0 ], false ) );
        // type
        params.add( std::make_unique<juce::AudioParameterInt>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 1 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 1 ], 1, 4, 1 ) );
        // rate
        params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 2 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 2 ], 0.00f, 20.0f, 1.0f ) );
        // depth
        params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 3 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 3 ], 0.0f, 1.0f, 0.25f ) );
        // offset
        params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 4 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 4 ], -1.0f, 1.0f, 0.0f ) );
        // triangle duty cycle
        params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 5 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 5 ], 0.0f, 1.0f, 0.5f ) );
        // sample and hold second rate
        params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 6 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 6 ], 0.0f, 20.0f, 1.0f ) );
        // temposync
        params.add( std::make_unique<juce::AudioParameterBool>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 7 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 7 ], false ) );
        // rate division
        
        params.add( std::make_unique<juce::AudioParameterInt>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 8 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 8 ], 1, 16, 1 ) );
        int nBeatNames = sjf_lfo::beatNames::sixteenthNote;
        params.add( std::make_unique<juce::AudioParameterInt>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 9 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 9 ], 1, nBeatNames, 1 ) );
        int nBeatTypes = sjf_lfo::beatTypes::septuplet; 
        params.add( std::make_unique<juce::AudioParameterInt>( juce::ParameterID{ lfoNames[ i ]+lfoParamNames[ 10 ], pIDVersionNumber }, LfoNames[ i ]+lfoParamNames[ 10 ], 1, nBeatTypes, 1 ) );
    }
    
    return params;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Sjf_moogLadderAudioProcessor();
}
