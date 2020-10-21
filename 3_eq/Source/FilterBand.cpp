#include "FilterBand.h"

void FilterBand::prepare(int numChannels)
{
    // Allocate required number of biquads
    if (numChannels != biquads.size())
        biquads.resize(numChannels);
    
    parameterValueChanged(0, 0);
}

// This function is called when values change
void FilterBand::parameterValueChanged (int parameterIndex, float newValue)
{
    const float freq = freqParam->get();
    const float qual = qualParam->get();
    const float gain = gainParam->get();
    
    const bool bandTypeChanged = (parameterIndex == typeParam->getParameterIndex());
    
    if (typeParam->getIndex() == 0)
    {
        for (int ch = 0; ch < biquads.size(); ++ch)
        {
            biquads[ch].design_lowpass_filter(freq, qual, samplerate);
            
            if (bandTypeChanged)
                biquads[ch].clearState(); // clear state only if band type was changed
        }
    }
    else if (typeParam->getIndex() == 1)
    {
        for (int ch = 0; ch < biquads.size(); ++ch)
        {
            biquads[ch].design_peaking_filter(freq, gain, qual, samplerate);
            
            if (bandTypeChanged)
                biquads[ch].clearState();
        }
    }
    else
    {
        // We shouldn't be here, somethings gone terribly wrong
        jassertfalse;
    }
}
