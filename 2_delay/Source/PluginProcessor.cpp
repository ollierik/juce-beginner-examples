#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayExampleAudioProcessor::DelayExampleAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                       )
{
    // add parameters
    delayLengthParam = new AudioParameterFloat("delayLength",       // internal name, host is using this to know which parameter it is
                                               "Delay Length (s)",  // this is the name that the user sees
                                               0.001,               // minimum value
                                               0.100,               // maximum value
                                               0.001);              // default
    
    modAmountParam = new AudioParameterFloat("modAmp",          // internal name, host is using this to know which parameter it is
                                             "Modulation (ms)", // this is the name that the user sees
                                             0,                 // minimum is 0 ms
                                             10,                // 10 ms
                                             1);                // default
    
    feedbackParam = new AudioParameterFloat("feedback", // internal name, host is using this to know which parameter it is
                                            "Feedback", // this is the name that the user sees
                                            0,          // minimum value is one second
                                            1,          // maximum value
                                            0.0);       // default
    
    lfoSpeedParam = new AudioParameterFloat("lfoSpeedParam",    // internal name, host is using this to know which parameter it is
                                            "LFO speed",        // this is the name that the user sees
                                            0,                  // minimum value
                                            1.0,                // maximum value
                                            0.5);               // default
    
    
    // on 0.1, we have 10% wet and 90% dry
    wetDryMixParam = new AudioParameterFloat("wetdrymix",   // internal name, host is using this to know which parameter it is
                                             "Wet/Dry Mix", // this is the name that the user sees
                                              0,            // minimum value is one second
                                              1,            // maximum value
                                              0.5);
    
    addParameter(delayLengthParam);
    addParameter(modAmountParam);
    addParameter(feedbackParam);
    addParameter(lfoSpeedParam);
    addParameter(wetDryMixParam);
    
}

DelayExampleAudioProcessor::~DelayExampleAudioProcessor()
{
}

//==============================================================================
const String DelayExampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayExampleAudioProcessor::acceptsMidi() const
{
    return false;
}

bool DelayExampleAudioProcessor::producesMidi() const
{
    return false;
}

bool DelayExampleAudioProcessor::isMidiEffect() const
{
    return false;
}

double DelayExampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayExampleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayExampleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayExampleAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayExampleAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayExampleAudioProcessor::changeProgramName (int index, const String& newName)
{
}

// We should create our delay lines and LFOs here, since this is the first occasion we'll know what the samplerate will be
void DelayExampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int maxDelayLineInSamples = 10 * sampleRate;
    
    // To assign a new object to std::unique_ptr, we call its reset().
    // This deletes the old object, if one exists, and moves to point to the new object.
    leftDelayLine.reset(new DelayLine(maxDelayLineInSamples));
    rightDelayLine.reset(new DelayLine(maxDelayLineInSamples));

    // Create our LFOs
    const double frequencyInHz = lfoSpeedParam->get();
    const double leftStartingPhase = MathConstants<double>::twoPi * 0;
    const double rightStartingPhase = MathConstants<double>::twoPi * 0.25;
    
    leftLfoOsc.reset(new SineOscillator(sampleRate, frequencyInHz, leftStartingPhase));
    rightLfoOsc.reset(new SineOscillator(sampleRate, frequencyInHz, rightStartingPhase));
    
    // These will be used for feedback, initialise to zero
    prevLeftDelayedSample = 0;
    prevRightDelayedSample = 0;
}

void DelayExampleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool DelayExampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // support for stereo-in-stereo-out
    if (layouts.getMainInputChannelSet() == AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == AudioChannelSet::stereo())
    {
        return true;
    }
    
    // support for mono-in-mono-out and mono-in-stereo-out
    if (layouts.getMainInputChannelSet() == AudioChannelSet::mono())
    {
        if (layouts.getMainOutputChannelSet() == AudioChannelSet::mono()
            || layouts.getMainOutputChannelSet() == AudioChannelSet::stereo())
        {
            return true;
        }
    }
    return false;
}


//////////////////////////////////////////////////////////
///////// MAGIC HAPPENS HERE
//////////////////////////////////////////////////////////

void DelayExampleAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    const int numInputs = getTotalNumInputChannels();
    const int numOutputs = getTotalNumOutputChannels();

    const float maxAmplitudeInSeconds = modAmountParam->get() / 1000;
    const float baseDelayInSeconds = delayLengthParam->get();
    const float wetDryRatio = wetDryMixParam->get();
    const double samplerate = getSampleRate();
    
    const float feedbackGain = feedbackParam->get();
    
    const int numSamplesInInput = buffer.getNumSamples();
    
    // Mono processing
    if (numInputs == 1 && numOutputs == 1)
    {
        // Update the frequency of the LFO
        const float lfoFreqInSecs = lfoSpeedParam->get();
        leftLfoOsc->setFrequency(lfoFreqInSecs);
        
        // Get access to the audio channel
        float* monoData = buffer.getWritePointer(0);

        // Process each sample
        for (int i = 0; i < numSamplesInInput; i++)
        {
            float inputSample = monoData[i];
            
            // Since we have an LFO, the delay in samples changes for every sample
            float delayInSamples = baseDelayInSeconds * samplerate + leftLfoOsc->getNextSample() * maxAmplitudeInSeconds * samplerate;

            // Push the sample to the delay line, and add the previous sample for the feedback effect
            leftDelayLine->pushSample(inputSample + prevLeftDelayedSample * feedbackGain);
            
            // Get the new delayed sample
            float wetSample = leftDelayLine->getDelayedSampleInterp(delayInSamples);
            float drySample = monoData[i];
            
            // Replace the output sample with a mixture of wet and dry samples
            monoData[i] = wetDryRatio * wetSample + (1-wetDryRatio) * drySample;
            
            // Store what the previous sample was to use it the next time around
            prevLeftDelayedSample = wetSample;
        }
    }
    // Stereo processing
    // This is essentially the mono version but doubled up for both channels
    // We could generalise and refactor the processing to a separate function and call it for both channels as needed
    // instead of copying the code like this.
    else if (numOutputs == 2)
    {
        // update the frequency of lfo
        const float lfoFreqInSecs = lfoSpeedParam->get();
        leftLfoOsc->setFrequency(lfoFreqInSecs);
        rightLfoOsc->setFrequency(lfoFreqInSecs);
        
        float* leftData = buffer.getWritePointer(0);
        float* rightData = buffer.getWritePointer(1);
        
        for (int i = 0; i < numSamplesInInput; i++)
        {
            float leftSample = leftData[i];
            float rightSample;
            
            // Handle stereo to stereo
            if (numInputs == 2)
            {
                rightSample = rightData[i];
            }
            // For mono to stereo, use the left channel input for right channel as well
            else
            {
                rightSample = leftSample;
            }
            
            float leftDelayInSamples = baseDelayInSeconds * samplerate + leftLfoOsc->getNextSample() * maxAmplitudeInSeconds*samplerate;
            float rightDelayInSamples = baseDelayInSeconds * samplerate + rightLfoOsc->getNextSample() * maxAmplitudeInSeconds*samplerate;

            leftDelayLine->pushSample(leftSample + prevLeftDelayedSample * feedbackGain);
            rightDelayLine->pushSample(rightSample + prevRightDelayedSample * feedbackGain);
            
            float leftWetSample = leftDelayLine->getDelayedSampleInterp (leftDelayInSamples);
            float rightWetSample = rightDelayLine->getDelayedSampleInterp (rightDelayInSamples);
            
            float leftDrySample = leftData[i];
            float rightDrySample = rightData[i];
            
            leftData[i] = wetDryRatio * leftWetSample + (1 - wetDryRatio) * leftDrySample;
            rightData[i] = wetDryRatio * rightWetSample + (1 - wetDryRatio) * rightDrySample;
            
            prevLeftDelayedSample = leftWetSample;
            prevRightDelayedSample = rightWetSample;
        }
    }
}

//==============================================================================
bool DelayExampleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayExampleAudioProcessor::createEditor()
{
    return new DelayExampleAudioProcessorEditor (*this);
}

//==============================================================================
void DelayExampleAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayExampleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayExampleAudioProcessor();
}
