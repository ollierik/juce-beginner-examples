#pragma once

#include <JuceHeader.h>

// This example demonstrates the minimum steps needed to use the juce::dsp's classes for audio processing in a plug-in.

class DspexampleAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    DspexampleAudioProcessor();
    ~DspexampleAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // Our processor inherits AudioProcessorParameter::Listener, and to do that, we also need to implement the next two methods.
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;
    
    // Let's separate parameter value updating to a separate function with an additional "all" flag.
    // The function is in charge of pushing a changed parameter value to a corresponding dsp processor setting.
    // The added bool flag can be used to force all parameters to propagate values, and is used in prepareToPlay().
    // The flag has a default value of false, so if not specified when calling the function, all won't be updated.
    void propagateParameterValue (int parameterIndex, bool all = false);

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    // Our parameters
    juce::AudioParameterFloat* thresholdParam;
    juce::AudioParameterFloat* ratioParam;
    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* releaseParam;
    juce::AudioParameterFloat* saturationParam;

    // An anonymous enum, that is, an enumeration without a name. Enumerations assign easy-to-remember names to index values.
    // If nothing else is specified, the first enumeration name gets the index 0, and all consecutive names get consecutive numbers, i.e. 1, 2, and 3.
    // In C++, an enumeration's names bleed into the parent scope. This can be problematic at times, but we can also use it to our adventage here.
    //
    // This enum is used for convenience to access the processors in the processorChain, defined below. Pay attention to have
    // the indices of the enum in the same order as the ProcessorChain members.
    enum {
        preSaturationGainIndex,
        waveshaperIndex,
        postSaturationGainIndex,
        compressorIndex
    };
    
    // A ProcessorChain links together several processors, and calls them one after another in the order they were declared.
    // Since it is a template class, the order can't be altered from the declaration order.
    //
    // WaveShaper as it is doesn't support additional parameters to the waveshaping function, so we apply gain and inverse gain before and after it.
    juce::dsp::ProcessorChain
    <
        juce::dsp::Gain<float>,
        juce::dsp::WaveShaper<float>,
        juce::dsp::Gain<float>,
        juce::dsp::Compressor<float>
    > processorChain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspexampleAudioProcessor)
};
