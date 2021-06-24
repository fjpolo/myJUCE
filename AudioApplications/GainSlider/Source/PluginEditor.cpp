/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainSliderAudioProcessorEditor::GainSliderAudioProcessorEditor (GainSliderAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    /*sliderAttach*/
    sliderAttach = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.treeState, GAIN_ID, gainSlider);

    ///*gainSlider - Linear*/
    //gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    //gainSlider.setRange(0.0, 1.0);
    //gainSlider.setValue(0.5);
    //gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    //gainSlider.addListener(this);
    //addAndMakeVisible(gainSlider);

    /*gainSlider - Log*/
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setRange(-48, 0.0);
    gainSlider.setValue(-1);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
}

GainSliderAudioProcessorEditor::~GainSliderAudioProcessorEditor()
{
}

//==============================================================================
void GainSliderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void GainSliderAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    /*gainSlider*/
    gainSlider.setBounds(getLocalBounds());
}

void GainSliderAudioProcessorEditor::sliderValueChanged(juce::Slider* Slider) {
    /*gainSlider*/
    if (Slider == &gainSlider) {
        /*Linear*/
        //audioProcessor.gainValue = gainSlider.getValue();
        /*Log*/
        audioProcessor.gainValue = pow(10, (gainSlider.getValue()/20.0));
        
    }
}