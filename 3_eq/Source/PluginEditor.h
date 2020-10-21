#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// A helper class to contain Sliders and attachments for a single band
struct EqBandComponent : public Component
{
    EqBandComponent(FilterBand& band);
    
    void paint (juce::Graphics& g) override;
    void resized() override;

    Slider freqSlider;
    Slider qualSlider;
    Slider gainSlider;
    
    SliderParameterAttachment freqAttachment;
    SliderParameterAttachment qualAttachment;
    SliderParameterAttachment gainAttachment;
};

class EqualiserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EqualiserAudioProcessorEditor (EqualiserAudioProcessor&);
    ~EqualiserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    EqualiserAudioProcessor& audioProcessor;
    
    EqBandComponent bandKnobs0;
    EqBandComponent bandKnobs1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualiserAudioProcessorEditor)
};
