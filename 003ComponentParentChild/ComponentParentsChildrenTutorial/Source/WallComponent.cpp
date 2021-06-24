/*
  ==============================================================================

    WallComponent.cpp
    Created: 31 May 2021 3:48:30pm
    Author:  fpolo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WallComponent.h"

//==============================================================================
WallComponent::WallComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WallComponent::~WallComponent()
{
}

void WallComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<float> wall(200, 200, 200, 120);
    g.fillCheckerBoard(wall, 20, 10, juce::Colours::sandybrown, juce::Colours::saddlebrown);

}

void WallComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
