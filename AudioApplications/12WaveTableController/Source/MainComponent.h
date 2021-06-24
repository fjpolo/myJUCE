#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent, public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void updateFrequency();

private:
    //==============================================================================
    /*GUI*/
    juce::Slider freqSlider;
    juce::Slider ampSlider;
    juce::Label freqLabel;
    juce::Label ampLabel;
    /*Audio*/
    juce::Array<float> waveTable;
    double waveTableSize;
    double frequency;
    double phase;
    double increment;
    double amplitude;
    double currentSamplerate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
