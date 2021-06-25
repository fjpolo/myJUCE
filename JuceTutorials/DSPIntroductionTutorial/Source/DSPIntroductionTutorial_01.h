/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             DSPIntroductionTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores the DSP module.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures, juce_dsp,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        DSPTutorialAudioProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

/**
* \brief References: https://docs.juce.com/master/tutorial_dsp_introduction.html
*/


#pragma once

//==============================================================================
template <typename Type>
class CustomOscillator
{
public:
    //==============================================================================
    /*Constructor*/
    CustomOscillator() {
        /**
        *   \brief Sine oscilator
        */
        // /*get a reference to the Oscillator by supplying the index of the process and use the processorChain.get<>() method*/
        // auto& osc = processorChain.template get<oscIndex>();
        // /*initialise the oscillator using a lambda function and the std::sin function to provide the sine wave to the oscillator*/
        // osc.initialise([](Type x) { return std::sin(x); }, 128);

        /**
        * \brief Triangular oscilator
        */
        auto& osc = processorChain.template get<oscIndex>();
        osc.initialise([](Type x)
            {
                return juce::jmap(x,
                    Type(-juce::MathConstants<double>::pi),
                    Type(juce::MathConstants<double>::pi),
                    Type(-1),
                    Type(1));
            }, 2);
    }

    //==============================================================================
    void setFrequency(Type newValue, bool force = false)
    {
        /*To set the frequency of the oscillator, we need to once again get a reference to it*/
        auto& osc = processorChain.template get<oscIndex>();
        osc.setFrequency(newValue, force);
    }

    //==============================================================================
    void setLevel(Type newValue)
    {
        /**/
        auto& gain = processorChain.template get<gainIndex>();
        gain.setGainLinear(newValue);
    }

    //==============================================================================
    void reset() noexcept {
        /* call the reset functions of each processor in the processor chain sequentially */
        processorChain.reset();
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        /*call the process functions of each processor in the processor chain sequentially*/
        processorChain.process(context);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        /*call the prepare functions of each processor in the processor chain sequentially*/
        processorChain.prepare(spec);
    }

private:
    //==============================================================================
    /*Processor indices - top-down order*/
    enum
    {
        oscIndex,
        gainIndex
    };
    /*juce::dsp::ProcessorChain*/
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<Type>, juce::dsp::Gain<Type>> processorChain;
};

//==============================================================================
class Voice : public juce::MPESynthesiserVoice
{
public:
    Voice()
    {
        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear(0.7f);

        /*Ladder Filter*/
        auto& filter = processorChain.get<filterIndex>();
        /*get the reference of the filter processor and set its cutoff frequency at 1kHz*/
        filter.setCutoffFrequencyHz(1000.0f);
        /*get the reference of the filter processor and set its resonance at 0.7*/
        filter.setResonance(0.7f);
        /**
         *  Exercise
         *      
         *       Try different resonance values and cutoff frequencies and listen to the output. 
         *      At the moment, the filter is a low-pass filter with 12dB/octave attenuation. 
         *      Can you make it a high-pass filter with 24dB/octave attenuation?
         */


        /*LFO*/
        lfo.initialise([](float x) { return std::sin(x); }, 128);
        lfo.setFrequency(3.0f);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare(spec);

        /*LFO*/
        /*Since we do not need to update the LFO as often as the audio processing sample rate, divide the sample rate by the LFO update rate to set the LFO sample rate in the prepare() function*/
        lfo.prepare({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
    }

    //==============================================================================
    void noteStarted() override
    {
        auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();

        /*OSC1*/
        processorChain.get<osc1Index>().setFrequency(freqHz, true);
        processorChain.get<osc1Index>().setLevel(velocity);

        /*OSC2*/
        /*Let's set the frequency of the second oscillator to the currently played note and pitch it up by 1% in the noteStarted() function*/
        processorChain.get<osc2Index>().setFrequency(freqHz * 1.01f, true);
        /*We can keep the velocity at the same lavel as the first oscillator*/
        processorChain.get<osc2Index>().setLevel(velocity);

        /*OSC3*/
        processorChain.get<osc3Index>().setFrequency(freqHz * 0.99f, true);
        processorChain.get<osc3Index>().setLevel(velocity);
    }

    //==============================================================================
    void notePitchbendChanged() override
    {
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<osc1Index>().setFrequency(freqHz);
        /*make sure that the detuned frequency remains the same when pitch bend is applied in the notePitchbendChanged() function*/
        processorChain.get<osc2Index>().setFrequency(freqHz * 1.01f);
        processorChain.get<osc3Index>().setFrequency(freqHz * 0.99f);
    }

    //==============================================================================
    void noteStopped(bool) override
    {
        clearCurrentNote();
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged()   override {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto output = tempBlock.getSubBlock(0, (size_t)numSamples);
        output.clear();

        /*In the following for() loop, we only modify the cutoff frequency every 100 samples.*/
        for (size_t pos = 0; pos < (size_t)numSamples;)
        {
            auto max = juce::jmin((size_t)numSamples - pos, lfoUpdateCounter);
            auto block = output.getSubBlock(pos, max);

            juce::dsp::ProcessContextReplacing<float> context(block);
            processorChain.process(context);

            pos += max;
            lfoUpdateCounter -= max;

            if (lfoUpdateCounter == 0)
            {
                lfoUpdateCounter = lfoUpdateRate;
                /*First call the processSample() function to process a single sample on the LFO*/
                auto lfoOut = lfo.processSample(0.0f);
                /* map its return value to the desired modulation range. We want to modulate the cutoff frequency from 100Hz to 2kHz.*/
                auto curoffFreqHz = juce::jmap(lfoOut, -1.0f, 1.0f, 100.0f, 2000.0f);
                /*Finally, apply the new cutoff frequency to the ladder filter*/
                processorChain.get<filterIndex>().setCutoffFrequencyHz(curoffFreqHz);
                /**
                *   Exercise
                *       Try to modulate different parameters such as filter resonance or oscillator frequency.          
                */
            }
        }

        juce::dsp::AudioBlock<float>(outputBuffer)
            .getSubBlock((size_t)startSample, (size_t)numSamples)
            .add(tempBlock);
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
        osc2Index,
        osc3Index,
        filterIndex,
        masterGainIndex
    };

    /*ProcessorChain*/
    //juce::dsp::ProcessorChain<CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;
    //juce::dsp::ProcessorChain<CustomOscillator<float>, CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;
    //juce::dsp::ProcessorChain<CustomOscillator<float>, CustomOscillator<float>, CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;
    juce::dsp::ProcessorChain<CustomOscillator<float>, CustomOscillator<float>, CustomOscillator<float>, juce::dsp::LadderFilter<float>, juce::dsp::Gain<float>> processorChain;
    /*LFO*/
    static constexpr size_t lfoUpdateRate = 100;
    size_t lfoUpdateCounter = lfoUpdateRate;
    juce::dsp::Oscillator<float> lfo;
};

//==============================================================================
class AudioEngine : public juce::MPESynthesiser
{
public:
    static constexpr auto maxNumVoices = 4;

    //==============================================================================
    AudioEngine()
    {
        for (auto i = 0; i < maxNumVoices; ++i)
            addVoice(new Voice);

        setVoiceStealingEnabled(true);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare(spec);

        /*Reverb*/
        fxChain.prepare(spec);
    }

private:
    /*create an effects chain in the AudioEngine class and add the juce::dsp::Reverb template type to the effects chain along with its index*/
    enum
    {
        reverbIndex
    };

    juce::dsp::ProcessorChain<juce::dsp::Reverb> fxChain;
    //==============================================================================
    void renderNextSubBlock(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock(outputAudio, startSample, numSamples);

        /*Reverb*/
        /**
         *  In order to process the effects chain, we need to retrieve the correct AudioBlock from the AudioBuffer 
         * to pass the context to the processing chain. First, convert the AudioBuffer to a usable AudioBlock
         */
        auto block = juce::dsp::AudioBlock<float>(outputAudio);
        /*refer to the right portion of the samples to manipulate using the getSubBlock() method */
        auto blockToUse = block.getSubBlock((size_t)startSample, (size_t)numSamples);
        /*get the processing context from this AudioBlock*/
        auto contextToUse = juce::dsp::ProcessContextReplacing<float>(blockToUse);
        /*process the effects chain with it*/
        fxChain.process(contextToUse);
    }
};

//==============================================================================
template <typename SampleType>
class AudioBufferQueue
{
public:
    //==============================================================================
    static constexpr size_t order = 9;
    static constexpr size_t bufferSize = 1U << order;
    static constexpr size_t numBuffers = 5;

    //==============================================================================
    void push(const SampleType* dataToPush, size_t numSamples)
    {
        jassert(numSamples <= bufferSize);

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy(buffers[(size_t)start1].data(), dataToPush, (int)juce::jmin(bufferSize, numSamples));

        abstractFifo.finishedWrite(size1);
    }

    //==============================================================================
    void pop(SampleType* outputBuffer)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy(outputBuffer, buffers[(size_t)start1].data(), (int)bufferSize);

        abstractFifo.finishedRead(size1);
    }

private:
    //==============================================================================
    juce::AbstractFifo abstractFifo{ numBuffers };
    std::array<std::array<SampleType, bufferSize>, numBuffers> buffers;
};

//==============================================================================
template <typename SampleType>
class ScopeDataCollector
{
public:
    //==============================================================================
    ScopeDataCollector(AudioBufferQueue<SampleType>& queueToUse)
        : audioBufferQueue(queueToUse)
    {}

    //==============================================================================
    void process(const SampleType* data, size_t numSamples)
    {
        size_t index = 0;

        if (state == State::waitingForTrigger)
        {
            while (index++ < numSamples)
            {
                auto currentSample = *data++;

                if (currentSample >= triggerLevel && prevSample < triggerLevel)
                {
                    numCollected = 0;
                    state = State::collecting;
                    break;
                }

                prevSample = currentSample;
            }
        }

        if (state == State::collecting)
        {
            while (index++ < numSamples)
            {
                buffer[numCollected++] = *data++;

                if (numCollected == buffer.size())
                {
                    audioBufferQueue.push(buffer.data(), buffer.size());
                    state = State::waitingForTrigger;
                    prevSample = SampleType(100);
                    break;
                }
            }
        }
    }

private:
    //==============================================================================
    AudioBufferQueue<SampleType>& audioBufferQueue;
    std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;
    size_t numCollected;
    SampleType prevSample = SampleType(100);

    static constexpr auto triggerLevel = SampleType(0.05);

    enum class State { waitingForTrigger, collecting } state{ State::waitingForTrigger };
};

//==============================================================================
template <typename SampleType>
class ScopeComponent : public juce::Component,
    private juce::Timer
{
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent(Queue& queueToUse)
        : audioBufferQueue(queueToUse)
    {
        sampleData.fill(SampleType(0));
        setFramesPerSecond(30);
    }

    //==============================================================================
    void setFramesPerSecond(int framesPerSecond)
    {
        jassert(framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz(framesPerSecond);
    }

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);

        auto area = getLocalBounds();
        auto h = (SampleType)area.getHeight();
        auto w = (SampleType)area.getWidth();

        // Oscilloscope
        auto scopeRect = juce::Rectangle<SampleType>{ SampleType(0), SampleType(0), w, h / 2 };
        plot(sampleData.data(), sampleData.size(), g, scopeRect, SampleType(1), h / 4);

        // Spectrum
        auto spectrumRect = juce::Rectangle<SampleType>{ SampleType(0), h / 2, w, h / 2 };
        plot(spectrumData.data(), spectrumData.size() / 4, g, spectrumRect);
    }

    //==============================================================================
    void resized() override {}

private:
    //==============================================================================
    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;

    juce::dsp::FFT fft{ Queue::order };
    using WindowFun = juce::dsp::WindowingFunction<SampleType>;
    WindowFun windowFun{ (size_t)fft.getSize(), WindowFun::hann };
    std::array<SampleType, 2 * Queue::bufferSize> spectrumData;

    //==============================================================================
    void timerCallback() override
    {
        audioBufferQueue.pop(sampleData.data());
        juce::FloatVectorOperations::copy(spectrumData.data(), sampleData.data(), (int)sampleData.size());

        auto fftSize = (size_t)fft.getSize();

        jassert(spectrumData.size() == 2 * fftSize);
        windowFun.multiplyWithWindowingTable(spectrumData.data(), fftSize);
        fft.performFrequencyOnlyForwardTransform(spectrumData.data());

        static constexpr auto mindB = SampleType(-160);
        static constexpr auto maxdB = SampleType(0);

        for (auto& s : spectrumData)
            s = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(s) - juce::Decibels::gainToDecibels(SampleType(fftSize))), mindB, maxdB, SampleType(0), SampleType(1));

        repaint();
    }

    //==============================================================================
    static void plot(const SampleType* data,
        size_t numSamples,
        juce::Graphics& g,
        juce::Rectangle<SampleType> rect,
        SampleType scaler = SampleType(1),
        SampleType offset = SampleType(0))
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight();

        auto center = rect.getBottom() - offset;
        auto gain = h * scaler;

        for (size_t i = 1; i < numSamples; ++i)
            g.drawLine({ juce::jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right)),
                          center - gain * data[i - 1],
                          juce::jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right)),
                          center - gain * data[i] });
    }
};

//==============================================================================
class DSPTutorialAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DSPTutorialAudioProcessor()
        : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {}

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        audioEngine.prepare({ sampleRate, (juce::uint32) samplesPerBlock, 2 });
        midiMessageCollector.reset(sampleRate);
    }

    void releaseResources() override {}

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return true;
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        midiMessageCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        audioEngine.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        scopeDataCollector.process(buffer.getReadPointer(0), (size_t)buffer.getNumSamples());
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override
    {
        return new DSPTutorialAudioProcessorEditor(*this);
    }

    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    //==============================================================================
    juce::MidiMessageCollector& getMidiMessageCollector() noexcept { return midiMessageCollector; }
    AudioBufferQueue<float>& getAudioBufferQueue() noexcept { return audioBufferQueue; }

private:
    //==============================================================================
    class DSPTutorialAudioProcessorEditor : public juce::AudioProcessorEditor
    {
    public:
        DSPTutorialAudioProcessorEditor(DSPTutorialAudioProcessor& p)
            : AudioProcessorEditor(&p),
            dspProcessor(p),
            scopeComponent(dspProcessor.getAudioBufferQueue())
        {
            addAndMakeVisible(midiKeyboardComponent);
            addAndMakeVisible(scopeComponent);

            setSize(400, 300);

            auto area = getLocalBounds();
            scopeComponent.setTopLeftPosition(0, 80);
            scopeComponent.setSize(area.getWidth(), area.getHeight() - 100);

            midiKeyboardComponent.setMidiChannel(2);
            midiKeyboardState.addListener(&dspProcessor.getMidiMessageCollector());
        }

        ~DSPTutorialAudioProcessorEditor() override
        {
            midiKeyboardState.removeListener(&dspProcessor.getMidiMessageCollector());
        }

        //==============================================================================
        void paint(juce::Graphics& g) override
        {
            g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        }

        void resized() override
        {
            auto area = getLocalBounds();
            midiKeyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
        }

    private:
        //==============================================================================
        DSPTutorialAudioProcessor& dspProcessor;
        juce::MidiKeyboardState midiKeyboardState;
        juce::MidiKeyboardComponent midiKeyboardComponent{ midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };
        ScopeComponent<float> scopeComponent;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DSPTutorialAudioProcessorEditor)
    };

    //==============================================================================
    AudioEngine audioEngine;
    juce::MidiMessageCollector midiMessageCollector;
    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector{ audioBufferQueue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DSPTutorialAudioProcessor)
};