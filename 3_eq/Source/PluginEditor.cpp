/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

EqBandComponent::EqBandComponent(FilterBand& band)
: freqSlider(Slider::RotaryVerticalDrag, Slider::TextBoxBelow)
, qualSlider(Slider::RotaryVerticalDrag, Slider::TextBoxBelow)
, gainSlider(Slider::RotaryVerticalDrag, Slider::TextBoxBelow)
, freqAttachment(*band.freqParam, freqSlider)
, qualAttachment(*band.qualParam, qualSlider)
, gainAttachment(*band.gainParam, gainSlider)
{
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(qualSlider);
    addAndMakeVisible(gainSlider);
}

void EqBandComponent::paint(juce::Graphics& g)
{
}

void EqBandComponent::resized()
{
    auto bounds = getLocalBounds();
    const int third = bounds.getWidth() / 3;
    
    freqSlider.setBounds(bounds.removeFromLeft(third));
    qualSlider.setBounds(bounds.removeFromLeft(third));
    gainSlider.setBounds(bounds);
}



/////////////////////////////////////////////////////////////////////////////////////////////


EqualiserAudioProcessorEditor::EqualiserAudioProcessorEditor (EqualiserAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, bandKnobs0(p.band0)
, bandKnobs1(p.band1)
{
    addAndMakeVisible(bandKnobs0);
    addAndMakeVisible(bandKnobs1);
    setSize (400, 300);
}

EqualiserAudioProcessorEditor::~EqualiserAudioProcessorEditor()
{
}

void EqualiserAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::green);
}

void EqualiserAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int h = bounds.getHeight() / 2;
    bandKnobs0.setBounds(bounds.removeFromTop(h));
    bandKnobs1.setBounds(bounds);
}
