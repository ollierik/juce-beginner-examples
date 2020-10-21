#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


// 7.
// ProcessorEditor is the GUI of the plug-in in JUCE parlance
// Our editor inherits AudioProcessorEditor, which in turn implements Component
// Components are the basis for anything that is drawn in JUCE.
class MidsideAudioProcessorEditor : public AudioProcessorEditor
{
public:
    MidsideAudioProcessorEditor (MidsideAudioProcessor&);
    ~MidsideAudioProcessorEditor();

    // 8.
    // Classes that inherit Component can override two essential functions, paint and resized.
    // paint is called for each Component to paint itself, and resized is called when the component is resized
    // Components can have child components, which should then be positioned in the resized() of the parent.
    void paint (Graphics&) override;
    void resized() override;

    // 9.
    // Our Slider object, and its SliderParameterAttachment that glues it into the slider
    Slider widthSlider;
    SliderParameterAttachment widthSliderAttachment;

    // 10.
    // This is a reference to the audio processor, to let the gui easily access the audio side of the plug-in.
    // References are a special type of pointers. The difference is, that it can't be nullptr, i.e. the reference has
    // to always be defined. It can be accessed like an object would be, i.e. with dot (.) operator instead of the arrow (->) operator.
    MidsideAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidsideAudioProcessorEditor)
};
