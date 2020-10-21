#include <JuceHeader.h>

#include "biquad.hpp"

// A helper struct to keep everything that we need for a single EQ band together
//
// This saves us from having to manually write all parameters for each band etc
// It is a template, and the template argument is of type int to generalise the number of channels we need
//
// We'll also make this class a listener of AudioProcessorParameters, i.e. it can receive information when
// parameter values change. This is done by inheriting a class defined in JUCE, in this case called
// AudioProcessorParameter::Listener. By inheriting it, out FilterBand can act as a listener, and should implement
// a function parameterValueChanged. To work, it has to register to be a listener.
struct FilterBand : public AudioProcessorParameter::Listener
{
    // Constructor of the struct. Is called when the FilterBand is created.
    //
    // Let's apply some template magic again!
    // To avoid having to bind the FilterBand to our specific example and make it more reusable, we define the AudioProcessorType as a
    // template parameter.
    //
    // This tells the compiler that "something" will be put here, and it'll try to sort it out after reading
    // all the files.
    //
    template <class AudioProcessorType>
    FilterBand(AudioProcessorType& processor, const String bandName, float defaultFreq, int defaultType, double& fs) : samplerate(fs)
    {
        const String bandId = bandName.replace(" ", "").toLowerCase(); // just in case
    
        // Add the parameters. We get the name as an argument. Make a id tag from it by removing whitespaces and pushing to lower case.
        // This isn't likely even needed, but old habits die hard.
        // The call order for AudioParameterFloat is:
        //     paramId, paramName, minVal, maxVal, defVal
    
        // Create parameters
        freqParam = new AudioParameterFloat (bandId + "freq", bandName + " Freq", 20, 20000, defaultFreq);
        qualParam = new AudioParameterFloat (bandId + "qual", bandName + " Q", 0.1, 16, 0.707);
        gainParam = new AudioParameterFloat (bandId + "gain", bandName + " Gain", -30, 30, 0);
    
        // Band type is a choice parameter. Google for docs.
        // To add more band types, add them to the list of types, and implement their respective design functions into parameterValueChanged
        typeParam = new AudioParameterChoice (bandId + "type", bandName + " Type", { "Lowpass", "Peaking", }, defaultType);
    
        // Add the newly created parameters to the audio processor
        processor.addParameter(freqParam);
        processor.addParameter(qualParam);
        processor.addParameter(gainParam);
        processor.addParameter(typeParam);
    
        // register as listener
        freqParam->addListener(this);
        qualParam->addListener(this);
        gainParam->addListener(this);
        typeParam->addListener(this);
    }

    FilterBand() = delete; // this means that a filter band can't be created with the so-called default constructor which has no parameters

    // We'll call this function in prepareToPlay(), it allocates our biquads
    void prepare(int numChannels);

    // This function is called when values change
    // We don't actually need the parameterIndex and newValue here, so commenting the names out removes "parameter unused" compiler warning
    void parameterValueChanged (int /* parameterIndex */, float /* newValue */) override;
    
    // This is part of the life of being a Parameter Listener, we need to implement unnecessary stuff due to library design in JUCE.
    // Just implement with empty body for now.
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    // The actual EQ state
    // Store the biquads into the processor state. Since the filter has a state that should be
    // carried over form block to block, we can't just create a new filter in every block.
    //
    // To make this more general, we're storing the biquads into a vector and allocate the memory for them.
    std::vector<Biquad<double>> biquads;
    
    // Parameters that the band needs
    AudioParameterFloat* freqParam;
    AudioParameterFloat* qualParam;
    AudioParameterFloat* gainParam;
    AudioParameterChoice* typeParam;
    
    double& samplerate; // let's store a reference of samplerate that the AudioProcessor maintains
};

