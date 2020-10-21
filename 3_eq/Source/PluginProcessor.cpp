#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
EqualiserAudioProcessor::EqualiserAudioProcessor()
: AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true).withOutput ("Output", juce::AudioChannelSet::stereo(), true))
, band0(*this, "Band 0", 1000, /* lowpass */ 0, samplerate) // call the constructors of FilterBands on initialisation
, band1(*this, "Band 1", 4000, /* peaking */ 1, samplerate)
{
}

EqualiserAudioProcessor::~EqualiserAudioProcessor()
{
}

//==============================================================================
const juce::String EqualiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EqualiserAudioProcessor::acceptsMidi() const
{
    return false;
}

bool EqualiserAudioProcessor::producesMidi() const
{
    return false;
}

bool EqualiserAudioProcessor::isMidiEffect() const
{
    return false;
}

double EqualiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EqualiserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EqualiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EqualiserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EqualiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void EqualiserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EqualiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // store this for later use
    this->samplerate = sampleRate;
    
    int numChannels = getTotalNumInputChannels();
    
    band0.prepare(numChannels);
    band1.prepare(numChannels);
}

void EqualiserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool EqualiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Accept mono-mono and stereo-stereo only
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono())
    {
        return true;
    }
    // Accept mono-mono and stereo-stereo only
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
    {
        return true;
    }
        
    return false;
}

void EqualiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals; // a boilerplate snippet from JUCE, potentially increases performance on some platforms
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer (ch);
        
        for (int i=0; i<numSamples; ++i)
        {
            // for easier reading, copy the sample from io buffer
            // this should be optimised by the compiler
            auto sample = channelData[i];
            
            // Replace sample with all bands, all bands run in series
            // There's an array of Biquad objects inside each band, with the name biquad.
            // A specific Biquad object per channel is then accessed with the [ch], and performFilter is called
            // It takes a sample as an input, and returns a sample, that we replace our "sample" with.
            sample = band0.biquads[ch].performFilter(sample);
            sample = band1.biquads[ch].performFilter(sample);
            
            // write back to io buffer
            channelData[i] = sample;
        }
    }
}

//==============================================================================
bool EqualiserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EqualiserAudioProcessor::createEditor()
{
    return new EqualiserAudioProcessorEditor (*this);
}

//==============================================================================
void EqualiserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EqualiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EqualiserAudioProcessor();
}
