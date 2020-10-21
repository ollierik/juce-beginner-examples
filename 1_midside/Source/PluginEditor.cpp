#include "PluginProcessor.h"
#include "PluginEditor.h"


// The constructor of our plug-in's GUI.
//
// From the second line onwards, we see a so-called intialiser list of syntax:
//      ClassConstructor() : Parent1(...), Parent2(...), ...., Member1(...), Member2(...), ....
// The initialiser list starts with a single colon character, followed by a list of parent constructor calls and member initialisations
// In this case, we first call the constructor of the parent class AudioProcessorEditor with parameter p, and then proceed to initialise
// the member fields that don't have a default constructor, i.e. a constructor which requires no parameters
// In this example, our SliderParameterAttachement is such.
//
// Please note, that since SliderParameterAttachment needs the information about the Slider that it attaches to, we have construct
// the Slider before the Attachment, and in the member declaration the Slider has to preceed the Attachment, since the order matters here!
MidsideAudioProcessorEditor::MidsideAudioProcessorEditor (MidsideAudioProcessor& p)
: AudioProcessorEditor (&p)
, widthSlider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow) // We can call any of the overloaded constructors that a class provides
, widthSliderAttachment(*p.widthParameter, widthSlider) // We have to provide the parameter and the slider to the slider attachement. Note the dereferenced parameter with *.
, processor (p)
{
    
    // We have to add the Slider to our parent to make it work
    addAndMakeVisible(widthSlider);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

MidsideAudioProcessorEditor::~MidsideAudioProcessorEditor()
{
}

void MidsideAudioProcessorEditor::paint (Graphics& g)
{
    // Fill the background with a colour
    g.fillAll (Colours::salmon);
}

void MidsideAudioProcessorEditor::resized()
{
    // We should set bounds to all our children, i.e. the Slider, here
    widthSlider.setBounds(50, 50, 100, 100);
}
