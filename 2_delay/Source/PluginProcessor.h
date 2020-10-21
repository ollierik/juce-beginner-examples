// Delay example audio processor
//
// This processor demonstrates a simple delay-based effect processor that
// could be adapted to work as a modulating delay, flanger, chorus by changing
// the delay and modulation times, and by adding more delay taps.
//
// The delay line reads have cubic interpolation

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayLine.h"
#include "SineOscillator.h"
class DelayExampleAudioProcessor : public AudioProcessor
{
public:
    DelayExampleAudioProcessor();
    ~DelayExampleAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
private:
    
    // std::unique_ptr is a smart pointer to an object
    // It will delete the object it points to when exiting, so no need to call:
    //      delete leftDelayLine;
    // in the destructor.
    std::unique_ptr<DelayLine> leftDelayLine;
    std::unique_ptr<DelayLine> rightDelayLine;
    
    std::unique_ptr<SineOscillator> leftLfoOsc;
    std::unique_ptr<SineOscillator> rightLfoOsc;

    // add pointers to parameters
    AudioParameterFloat* delayLengthParam;
    AudioParameterFloat* modAmountParam;
    AudioParameterFloat* feedbackParam;
    AudioParameterFloat* lfoSpeedParam;
    AudioParameterFloat* wetDryMixParam;
    
    // member variables
    float prevLeftDelayedSample;
    float prevRightDelayedSample;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayExampleAudioProcessor)
};










