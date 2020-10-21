#pragma once
// ^ always put that in the .h file

#include "../JuceLibraryCode/JuceHeader.h"

// A simple delay line with interpolated reads.
class DelayLine
{
public:
    
    // Constructor
    // Has to allocate the buffer that we'll be using to store the samples
    DelayLine(int maxNumSamples);
    
    // destructor
    ~DelayLine();
    
    // Push a new sample into the delay line
    void pushSample(double sample);
    
    // Get a sample that has been delayed delayInSamples amount
    double getDelayedSample(int delayInSamples);
    
    // Get a sample, but interpolated. We need to use this if the delay read tap moves
    double getDelayedSampleInterp(float delayInSamples);

    
private:
    
    AudioBuffer<double>* buffer;
    
    int writehead;
    const int maxNumSamples;
};






