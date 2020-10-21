#include "DelayLine.h"

// Wrap an integer to a range
int wrapToRange(int number, int min, int max)
{
    if (number < min) number += max;
    if (number >= max) number -= max;
    return number;
}

// we have to use *initializer list* to assign the value to maxNumSamples
// the single ':' means, that we are initialising the fields of our object
DelayLine::DelayLine (int numSamplesForTheDelayLine)
: maxNumSamples(numSamplesForTheDelayLine)
{
    buffer = new AudioBuffer<double> (1, maxNumSamples);
    writehead = 0;
    buffer->clear();
}

DelayLine::~DelayLine()
{
    delete buffer;
}

void DelayLine::pushSample(double sample)
{
    buffer->setSample(0, writehead, sample);
    writehead += 1;
    
    // if playhead goes out-of-bounds, i.e. exceeds the maxNumSamples
    // reset it back to 0
    if (writehead >= maxNumSamples)
    {
        writehead = 0;
    }
}

// This is the non-interpolating delay read function for reference.
// In this example plug-in we're not using this one
double DelayLine::getDelayedSample(int delayInSamples)
{
    // cap delayInSamples to 0
    if (delayInSamples < 0)
        delayInSamples = 0;
    
    // Correct a too long delay time
    if (delayInSamples >= maxNumSamples)
        delayInSamples = maxNumSamples;
    
    int tapindex = writehead - delayInSamples;
    
    // if tapindex goes out-of-bounds, i.e. below 0, continue from the maxNumSamples downwards
    
    tapindex = wrapToRange(tapindex, 0, maxNumSamples);
    
    // Hoorraah, we've made sure that our tap index is not out-of-bounds.
    //
    // Or have we?
    // If tap index is still below 0, we are asking for too many samples to begin with.
    // How should we handle this?
    //
    // This only applies for a debug build:
    // If the condition inside the jassert is not true, we're getting a soft crash
    jassert(tapindex >= 0);
    
    return buffer->getSample(0, tapindex);
}

double DelayLine::getDelayedSampleInterp(float delayInSamples)
{
    int delayInSamplesInt = (int)delayInSamples;
    
    // cap the delay in samples to 1, otherwise we'd need audio information from the future
    if (delayInSamplesInt < 1)
        delayInSamplesInt = 1;

    int tapindex = writehead - delayInSamples;
    
    int aIndex = tapindex - 1;
    int bIndex = tapindex;
    int cIndex = tapindex + 1;
    int dIndex = tapindex + 2;
    
    // wrap all indices
    aIndex = wrapToRange(aIndex, 0, maxNumSamples);
    bIndex = wrapToRange(bIndex, 0, maxNumSamples);
    cIndex = wrapToRange(cIndex, 0, maxNumSamples);
    dIndex = wrapToRange(dIndex, 0, maxNumSamples);
    
    const float a = buffer->getSample(0, aIndex);
    const float b = buffer->getSample(0, bIndex);
    const float c = buffer->getSample(0, cIndex);
    const float d = buffer->getSample(0, dIndex);

    const float fract = delayInSamples - delayInSamplesInt;
    
    // This line of magic just calculates the interpolated value based on a weighted sum,
    // based on four consecutive sample values a to d.
    // The code snippet is from the source code of puredata's tabread4~ object.
    float cminusb = c-b;
    float interpValue = b + fract * ( cminusb - 0.1666667f * (1.-fract) * ( (d - a - 3.0f * cminusb) * fract + (d + 2.0f*a - 3.0f*b)));

    return interpValue;
}








