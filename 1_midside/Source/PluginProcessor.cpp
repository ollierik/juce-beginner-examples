// 7.
// This file implements the functionality declared in PluginProcessor.h
//
// Top of the file typically has includes. C++ compiler is pretty archaic with these, and an include has little smarts in it.
// An include actually just copies the contents of the file that was included in place of the include statement, so order does matter.

#include "PluginProcessor.h"
#include "PluginEditor.h"

// A word of note:
// This example was generated with Projucer. The template that the Projucer creates has a set of preprocessor statements,
// such as #ifdef, #ifndef, and their respective #endifs
// In this example, all of them have been cleaned to reflect this plug-in for easier readability

// 2.
// Implementation of the constructor
// Here we can do stuff like initialising variables to specific values.
// The syntax ClassName::ClassMethod is used to implement methods that have been declared elsewhere.
MidsideAudioProcessor::MidsideAudioProcessor()
     : AudioProcessor (BusesProperties().withInput ("Input",  AudioChannelSet::stereo(), true).withOutput ("Output", AudioChannelSet::stereo(), true)
                       )
{
    // Parameters are created in the constructor.
    // For new projects and complex plug-ins, JUCE recommends us to use a more complex way of creating parameters.
    //
    // For accessibility, however, we'll be using the older and simpler way in these examples.
    // Please refer to JUCE's tutorials and documentations on how to initialise parameters with groups and with AudioProcessorValueTreeState.
    
    // Create our widthParameter. It is of type AudioParameterFloat, meaning that it is intended to hold a floating-point value.
    // The keyword "new" means that we allocate memory for it. We then proceed to storing the memory location to our member variable widthParameter
    // to access the parameter later.
    widthParameter = new AudioParameterFloat ("widthparam", // parameter ID, used internally
                                              "Width",      // parameter name, visible to the end-user
                                              0.0f,       // minimum value
                                              1.0f,         // maximum value
                                              0.5f);        // default value
    
    // Add the parameter to the host. The host takes ownership of the parameter, so we don't have to delete its memory on destruction.
    addParameter (widthParameter);
}

// Implementation of the destructor. This will be called, before the plug-in closes. We don't do anything fancy, so the implementation is empty.
MidsideAudioProcessor::~MidsideAudioProcessor()
{
}

// This function will return the name of the plugin
// The name is of data type const String
// The function is const (hence the trailing keyword)
// this means, that it can't change the state of the object
const String MidsideAudioProcessor::getName() const
{
    return JucePlugin_Name; // this is a Macro, defined by the Projucer on export. Google for more info on C/C++ macros.
}

bool MidsideAudioProcessor::acceptsMidi() const
{
    return false; // no, don't want any
}

bool MidsideAudioProcessor::producesMidi() const
{
    return false;
}

bool MidsideAudioProcessor::isMidiEffect() const
{
    return false;
}

double MidsideAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// How many presets?
int MidsideAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

// What factory preset are we on?
int MidsideAudioProcessor::getCurrentProgram()
{
    return 0;
}

// Change to a factory preset
void MidsideAudioProcessor::setCurrentProgram (int index)
{
}

// Get the name of a factory preset
const String MidsideAudioProcessor::getProgramName (int index)
{
    return {};
}

// We never do that, as we don't have any
void MidsideAudioProcessor::changeProgramName (int index, const String& newName)
{
}

// 3.
// Here we initialise anything we couldn't initialise in the constructor.
// We could for example precompute temporaries or stuff that we need in the dsp.
// We also want to reserve all the memory that our algorithm is going to need.
void MidsideAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void MidsideAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

// 4.
// The host repeatedly asks our plug-in for different layout combinations, and we're supposed to return
// true if such particular channel and bus layout is supported.
//
// Our plug-in is a stereo-in-stereo-out-only plug-in, so that's the only one that we're going to accept:
bool MidsideAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const bool inputIsStereo = (layouts.getMainInputChannelSet() == AudioChannelSet::stereo());
    const bool outputIsStereo = (layouts.getMainOutputChannelSet() == AudioChannelSet::stereo());
    
    return inputIsStereo && outputIsStereo;
}


// 5.
// This is where the magic happens.
// Return type is void, i.e. our function doesn't "return" anything.
// Input is two Juce containers. AudioBuffer buffer holds the (multichannel) audio data.
// The other one holds the midi data, which we don't do anything with now.
// AudioBuffer<float> is an object, that contains audio samples, which are of type float.
//
// The processBlock is expected to replace the contents of the buffer object with its own output

void MidsideAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    // let's find the address to our left and rigth channel data
    // These pointers now point to the first sample for each array of numbers that represent the audio
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    // number of samples in the block
    const int numSamples = buffer.getNumSamples();
    
    // Get the width parameter value and store it to a value "sideGain"
    const float sideGain = widthParameter->get();
    // Also precalculate a variable for gaining the mid
    const float midGain = 1.0f - sideGain;

    // Audio processing is typically done with "for loop"s.
    //
    // A for loop declaration comes in three parts:
    //
    // 1. Define the variables:
    //      int i = 0;
    //
    // 2. Define the condition that breaks the loop:
    //      if (i < numSamples), break the loop
    // In other words, continue the loop as long as the condition is true
    //
    // 3. Increment variable values, i.e. advance the loop
    //      i += 1;
    //
    // So lets iterate through all audio sample points in the buffer

    for (int i = 0; i < numSamples; i++)
    {
        // Array access syntax [] can be used for pointers.
        // Since both leftChannel and rightChannel point to the first sample of their respective arrays, we can access
        // the successive sample with an integer (= int) value.
        const float leftSample = leftChannel[i]; // get the i'th value of left channel, etc.
        const float rightSample = rightChannel[i];
        
        // Calculate the mid and side at index i from the left and right sample at index i
        float midSample = leftSample + rightSample;
        float sideSample = leftSample - rightSample;
        
        // Manipulate mid and side samples with the width
        midSample *= midGain;
        sideSample *= sideGain;
        
        // Convert back to LR representation
        float outputLeftSample = midSample + sideSample;
        float outputRightSample = midSample - sideSample;
        
        // Replace the values of leftChannel and rightChannel at index i
        leftChannel[i] = outputLeftSample;
        rightChannel[i] = outputRightSample;
    }
}

bool MidsideAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MidsideAudioProcessor::createEditor()
{
    return new MidsideAudioProcessorEditor (*this);
}

void MidsideAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidsideAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// JUCE boilerplate function that has to exist
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidsideAudioProcessor(); // This creates new instances of the plugin.
    
}



////////////////////////////////////////////////////////////////////////////////
// 6.
// Pointer example just for the sake of it:
void pointerExample(String day)
{
    // Here we have two numbers
    int mondayNumber = 5;
    int typicalNumber = 6;
    
    // This fellow points to one of them.
    // It is a pointer, i.e. an address to a memory location
    // the "int" in front of it means, that it will interpret the contents of the memory location as an int
    //
    // We assign the value of "nullptr", i.e. 0, to our pointer. If not assigned to anything, a pointer has random crap as its value.
    // Nullptr is a "special" value that is used to indicate, that our pointer does not point to anything.
    int* pointerToOurFavouriteNumber = nullptr;

    // If the day is Monday, our favourite number is going to be located in
    // a variable "mondayNumber". The &-sign in front of number takes the address of that variable.
    if (day == "Monday")
    {
        pointerToOurFavouriteNumber = &mondayNumber;
    }
    // Otherwise, if not Sunday, our favourite number will be located in "typicalNumber"
    else if (day != "Sunday")
    {
        pointerToOurFavouriteNumber = &typicalNumber;
    }
    
    // To access the value that was stored into the pointer, we have to derefrence it.
    // To dereference a pointer, we use the asterisk character. Oh boy, talk about confusing syntax for beginners!
    int ourFavouriteNumberValue = *pointerToOurFavouriteNumber;
    
    // This prints the value to the console à la JUCE
    DBG(ourFavouriteNumberValue);
    
    // One more thing:
    pointerToOurFavouriteNumber = nullptr;
    int test = *pointerToOurFavouriteNumber;
    // ^ dereferencing a nullptr value crashes our program. So before dereferencing or using a value, we should ALWAYS check for its correctness.
    // This is typically done:
    //      if (pointer != nullptr)
    //          ... do something ...
    // You may also encounter shorter syntax:
    //      if (pointer)
    //          ... do something ...
    // Since nullptr = 0, if (0) is false
    
}

