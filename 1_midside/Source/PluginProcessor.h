#pragma once // this line tells the compiler that this file is to be included only once, and that we can disregard this file's include if done more than once

#include "../JuceLibraryCode/JuceHeader.h"


// 1.
// Our plugin is a class that inherits AudioProcessor - this is indicated by the
//      : public AudioProcessor
// This means that our plug-in's audio processor class can appear as an AudioProcessor, and can (or has to)
// implement a set of functions to work as an audio processor.
//
// In C++, a class declaration is typically written into a file with .h or .hpp ending. This declaration describes
// our class, its methods and member fields (see 6.). The declaration is then implemented in a .cpp file.
//
// This way we can for example provide a different implementation for different platforms (e.g. Mac or Windows).
// The separation of declaration and implementation has historical reasons, and lets us avoid some pitfalls of C++,
// known as cyclical dependencies, where two classes may require information about each other. More on this in the later examples.
class MidsideAudioProcessor : public AudioProcessor
{
public: // any other code can interact with public mehtods and member variables of a class

    // 2.
    // Constructor:
    // This function is always called, when we create a new object of the type MidsideAudioProcessor.
    // Constructor always has the same name as the class does.
    MidsideAudioProcessor();
    
    // Destructor
    // This will be automatically called before deleting the object.
    // Lucky for us, we try very hard not to delete anything, so in this example it isn't really needed.
    // Destructors name has the tilde character before the class name
    
    ~MidsideAudioProcessor();

    // 3.
    // This method (method = function of a class) is called before we start processing any audio.
    // Until this point, the samplerate is not known, so prepareToPlay is the place to prepare our audio processing,
    // and for example allocate any memory.
    //
    // The "override" keyword after most of these functions means, that the method is declared in the parent class that we inherit,
    // i.e. AudioProcessor, and that our class has a specialised implementation for it.
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    
    // Called when playback ends, we can free any allocated memory here and do a general clean-up if needed
    void releaseResources() override;

    // 4.
    // This function is used to determine what bus layouts are supported, such as mono in mono out, or stereo in mono out etc.
    // See the implementation in PluginProcessor.cpp for more.
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // 5.
    // The magic happens in processBlock, it is called to process our audio.
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // GUI-related methods
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // Other general stuff
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // Program (i.e. "preset") -related stuff
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    // These two are used by hosts to store the internal state of the plug-in, so that it can be recalled when opening a session later on.
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // 6.
    // Member variables, also known as member fields
    // In this example, we only have one. The asterisk after the typename of the variable makes it a pointer.
    // Pointers are memory addresses that point to somewhere in the memory. So the actual object is not actually
    // stored inside our MidSideAudioProcessor, but rather, we can have a handle to access it later on.
    //
    // You can think of pointers as fishing rods, where the hook, the actual object, is actually located elsewhere. When needed, we can follow the
    // line from the pointer to the memory that contains the object that the pointer is pointing to.
    //
    // There's a more elaborate example on pointers at the bottom of PluginProcessor.h to get you started.
    AudioParameterFloat* widthParameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidsideAudioProcessor)
};
