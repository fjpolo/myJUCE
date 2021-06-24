#include "MainComponent.h"

using namespace juce;

//==============================================================================
MainComponent::MainComponent()
{
    /*GUI*/
    setSize(800, 600);

    /*freqSlider*/
    //freqSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    freqSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);

    freqSlider.setRange(440, 880);
    freqSlider.setTextValueSuffix("[Hz]");
    freqSlider.addListener(this);
    freqSlider.setValue(440);
    freqLabel.setText("Frequency", juce::dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);
    addAndMakeVisible(freqSlider);

    /*ampSlider*/
    //ampSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    ampSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    ampSlider.setRange(0.0f, 1.0f);
    ampSlider.setTextValueSuffix("[]");
    ampSlider.addListener(this);
    ampSlider.setValue(0.125);
    ampLabel.setText("Amplitude", juce::dontSendNotification);
    ampLabel.attachToComponent(&ampSlider, true);
    addAndMakeVisible(ampSlider);

    

    /*Audio*/
    setAudioChannels(0, 2); // no inputs, stereo output

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    /*Audio*/
    frequency = 440;
    phase = 0;
    waveTableSize = 1024;
    amplitude = 0.125;
    currentSamplerate = sampleRate;
    increment = frequency * waveTableSize / currentSamplerate;
    /*One cycle sine*/
    for (auto i = 0;i < waveTableSize;++i) {
        waveTable.insert(i, std::sin(2.0 * juce::MathConstants<double>::pi * i / waveTableSize));
    }

}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    float* const bufferL = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* const bufferR = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    for (auto sample = 0;sample < bufferToFill.numSamples;++sample) {
        auto temp = waveTable[(int)phase] * amplitude;
        bufferL[sample] = temp;
        bufferR[sample] = temp;
        updateFrequency();        
    }
}

void MainComponent::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    const int labelSpace{ 100 };
    freqSlider.setBounds(labelSpace, 100, getWidth() - 100, 100);
    ampSlider.setBounds(labelSpace, 200, getWidth() - 100, 100);
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &freqSlider) {
        frequency = freqSlider.getValue();
    }
    if (slider == &ampSlider) {
        amplitude = ampSlider.getValue();
    }
}

void MainComponent::updateFrequency() {
    increment = frequency * waveTableSize / currentSamplerate;
    phase = fmod(phase + increment, waveTableSize);
}
