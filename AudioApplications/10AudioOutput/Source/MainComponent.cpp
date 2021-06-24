#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(800, 600);
    setAudioChannels(0, 2); // no inputs, stereo output
    //setAudioChannels(0, 1); // no inputs, mono output
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::String message;
    message << "Preparing to play audio...\n";
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
    message << " sampleRate = " << sampleRate;
    juce::Logger::getCurrentLogger()->writeToLog(message);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    /*Generate PCM signal for stereo sound*/
    for (int channel = 0; channel < (bufferToFill.buffer->getNumChannels()); ++channel) {
        /*Point buffer to the first element of buffetToFill*/
        float* const buffer = bufferToFill.buffer->getWritePointer(channel, 0);
        /*Iterate through bufferToFill*/
        for (int sample = 0; sample < (bufferToFill.numSamples); ++sample) {
            /*Generate the signal*/
            buffer[sample] = (random.nextFloat() * 2 - 1.0f) * 0.25f;

        } // END for sample
    } // END for channel
}

void MainComponent::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
