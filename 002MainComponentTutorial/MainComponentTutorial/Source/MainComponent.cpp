#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(400, 300);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::lightblue);
    g.setColour(juce::Colours::darkblue);
    g.setFont(20.0f);
    //g.drawText("Hello, World!", getLocalBounds(), juce::Justification::centred, true);
    g.drawText(currentSizeAsString, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    currentSizeAsString = juce::String(getWidth()) + " x " + juce::String(getHeight());
}
