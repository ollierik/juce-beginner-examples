#pragma once

// remember this!
#include "../JuceLibraryCode/JuceHeader.h"

// A simple sine oscillator that we use for our LFOs
class SineOscillator
{
public:
    SineOscillator(double samplerate, double freq, double initialPhase);
    
    // Note! No destructor a.k.a. ~SineOscillator, because we don't need to assign and delete memory
    
    // Get current frequency
    double getFrequency();

    // Set a new frequency
    void setFrequency(double newFrequency);

    // Get next output sample determined by the frequency and currentPhaseState.
    // This also increments the state.
    double getNextSample();

private:

    double samplerate;
    double frequency;
    
    // current phase state should wrap around 2*pi
    double currentPhaseState;
};
