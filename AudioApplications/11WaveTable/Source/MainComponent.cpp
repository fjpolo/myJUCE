#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(800, 600);
    setAudioChannels(0, 2); // no inputs, stereo output
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    frequency = 440;
    phase = 0;
    waveTableSize = 1024;
    amplitude = 0.125;
    increment = frequency * waveTableSize / sampleRate;
    /*One cycle sine*/
    for (auto i = 0;i < waveTableSize;++i) {
        waveTable.insert(i, std::sin(2.0 * juce::MathConstants<double>::pi * i / waveTableSize));
    }
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    float* const bufferL = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* const bufferR = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    for (auto sample = 0;sample < bufferToFill.numSamples;++sample) {
        bufferL[sample] = waveTable[(int)phase] * amplitude;
        bufferR[sample] = bufferL[sample];
        phase = fmod(phase + increment, waveTableSize);
    }
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
