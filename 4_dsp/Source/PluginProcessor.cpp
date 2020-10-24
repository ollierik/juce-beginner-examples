#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DspexampleAudioProcessor::DspexampleAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                       )
{
    // To save space, we can combine creating a parameter with adding the parameter
    // In C++, an equal operator '=' typically returns a reference to the value that was assigned.
    addParameter(saturationParam = new AudioParameterFloat("saturation", "Saturation", 0.01, 100, 1));
    addParameter(thresholdParam = new AudioParameterFloat("threshold", "Threshold (dB)", -60, 0, -10));
    addParameter(ratioParam = new AudioParameterFloat("ratio", "Ratio", 1, 20, 4));
    addParameter(attackParam = new AudioParameterFloat("attack", "Attack (ms)", 1, 30, 12));
    addParameter(releaseParam = new AudioParameterFloat("release", "Release (ms)", 1, 300, 150));

    // Register our AudioProcessor as a listener of the parameters
    saturationParam->addListener(this);
    thresholdParam->addListener(this);
    ratioParam->addListener(this);
    attackParam->addListener(this);
    releaseParam->addListener(this);
}

DspexampleAudioProcessor::~DspexampleAudioProcessor()
{
}

//==============================================================================
const juce::String DspexampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DspexampleAudioProcessor::acceptsMidi() const
{
    return false;
}

bool DspexampleAudioProcessor::producesMidi() const
{
    return false;
}

bool DspexampleAudioProcessor::isMidiEffect() const
{
    return false;
}

double DspexampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DspexampleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DspexampleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DspexampleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DspexampleAudioProcessor::getProgramName (int index)
{
    return {};
}

void DspexampleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void DspexampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // We need to provide a ProcessSpec to our ProcessorChain so that they know the samplerate and such.
    // Here we define a variable of type ProcessSpec and name spec, and use a struct initialiser recognised by the curly brackets.
    // This assigns the provided values in the order they are in the struct. Then we give the struct to .prepare() method.
    
    juce::dsp::ProcessSpec spec { sampleRate, (uint32)samplesPerBlock, (uint32)getTotalNumInputChannels() };
    processorChain.prepare(spec);
    
    // Now that we have setup the processors, we can push all parameter values to their corresponding settings.
    // Let's use the bool flag in propagateParameterValue to do this.
    propagateParameterValue(-1, true);
    
    // Get an easier handle to the waveshaper. Get the corresponding dsp module from the processorChain with get<i>(), where
    // i is the index of the processor.
    //
    // C++ always requires a type of a variable when declared, but 'auto' can be used to automatically deduce the type. By using auto&, we instruct the compiler
    // that the auto should be a reference instead of a value. Be warned, if you forget the &, the compiler may copy the dsp module
    // instead of getting a reference to it, and all of the changes would be applied to the copy instead of the original, i.e. nothing would be changed.
    auto& waveshaper = processorChain.get<waveshaperIndex>();
    
    // Set the function to use for the distortion. The syntax used here is the lambda syntax, i.e. it is a function without a name.
    waveshaper.functionToUse = [] (float x)
    {
        return std::tanh (x);
    };
}

void DspexampleAudioProcessor::releaseResources()
{
    processorChain.reset();
}

bool DspexampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono())
    {
        return true;
    }
    
    return false;
}

void DspexampleAudioProcessor::parameterValueChanged (int parameterIndex, float /*newValue*/)
{
    // newValue is normalised to [0, 1] range, since that is the native way to handle them.
    // We can get the non-normalised value of a parameter by asking it directly from a paremeter,
    // so we can discard the normalised value
    propagateParameterValue (parameterIndex);
}

void DspexampleAudioProcessor::propagateParameterValue(int parameterIndex, bool all)
{
    // A simple way to propagate each parameter's value to the corresponding setting.
    // There would be a number of more elegant, and perhaps less CPU-heavy, ways to achieve this,
    // but with a small number of parameters, this should suffice.
    // Each if exits the function if the "all" flag was not specified.
    //
    // To get the value from the parameter, we can use a feature in the AudioParameterFloat.
    // It has a float operator defined, meaning that it can appear as a float to an expression if that
    // makes sense to the compiler, i.e. if it is not ambiguous.
    //
    // Since our parameters are pointers, we first need to dereference each parameter with an asterisk '*'.
    // This produces the same result as calling for example saturationParam->get().
    
    if (parameterIndex == saturationParam->getParameterIndex() || all)
    {
        const float saturationGain = *saturationParam;
        processorChain.get<preSaturationGainIndex>().setGainLinear(saturationGain);
        processorChain.get<postSaturationGainIndex>().setGainLinear(1.0f / saturationGain);
        if (!all) return;
    }
    if (parameterIndex == thresholdParam->getParameterIndex() || all)
    {
        processorChain.get<compressorIndex>().setThreshold(*thresholdParam);
        if (!all) return;
    }
    if (parameterIndex == ratioParam->getParameterIndex() || all)
    {
        processorChain.get<compressorIndex>().setRatio(*ratioParam);
        if (!all) return;
    }
    if (parameterIndex == attackParam->getParameterIndex() || all)
    {
        processorChain.get<compressorIndex>().setAttack(*attackParam);
        if (!all) return;
    }
    if (parameterIndex == releaseParam->getParameterIndex() || all)
    {
        processorChain.get<compressorIndex>().setRelease(*releaseParam);
        if (!all) return;
    }
}

// Body of the function intentionally left blank
void DspexampleAudioProcessor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting) {}

void DspexampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // To use the dsp modules, we first need to wrap the audio buffer into an AudioBlock<float> object.
    // The AudioBlock is then wrapped inside a context. Several types of contexts exist, here we're
    // using ProcessContextReplacing, which replaces the samples of the buffer with processed samples.
    
    juce::dsp::AudioBlock<float> audioBlock (buffer);
    juce::dsp::ProcessContextReplacing<float> context (audioBlock);

    processorChain.process(context);
}

//==============================================================================
bool DspexampleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DspexampleAudioProcessor::createEditor()
{
    return new DspexampleAudioProcessorEditor (*this);
}

//==============================================================================
void DspexampleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DspexampleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DspexampleAudioProcessor();
}
