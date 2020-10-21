#include "SineOscillator.h"

SineOscillator::SineOscillator(double sr, double freq, double initialPhase)
{
    samplerate = sr;
    frequency = freq;
    currentPhaseState = initialPhase;
}

double SineOscillator::getFrequency()
{
    return frequency;
}

void SineOscillator::setFrequency(double newFrequency)
{
    // frequency can't be above nyquist or below 0
    const double nyquist = samplerate/2;
    
    // first, let's check that the new frequency is not below 0
    if (newFrequency < 0)
    {
        // if new frequency is below 0, mirror it back to positive axis
        newFrequency = -newFrequency;
    }
    
    // if we get here, check if our frequency is not above nyquist
    if (newFrequency > nyquist)
    {
        // if above nyquist, cap it to nyquist
        frequency = nyquist;
    }
    else
    {
        frequency = newFrequency;
    }
}

double SineOscillator::getNextSample()
{
    double phaseIncrement = frequency / samplerate * MathConstants<double>::twoPi;
    
    currentPhaseState += phaseIncrement;
    
    // wrap the phase
    if (currentPhaseState > MathConstants<double>::twoPi)
        currentPhaseState -= MathConstants<double>::twoPi;
    
    double oscillation = cos(currentPhaseState);
    return oscillation;
}








