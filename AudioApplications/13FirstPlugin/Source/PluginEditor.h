/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class _13FirstPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    _13FirstPluginAudioProcessorEditor (_13FirstPluginAudioProcessor&);
    ~_13FirstPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    _13FirstPluginAudioProcessor& audioProcessor;
    /*User defined*/
    juce::Slider gainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_13FirstPluginAudioProcessorEditor)
};
