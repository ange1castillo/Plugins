#pragma once

#include <JuceHeader.h>
#include "Synthesizer/SynthSound.h"

class SynthVoice : public SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice() override;
    bool canPlaySound (SynthesiserSound*) override;
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    void renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);

private:
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    juce::AudioBuffer<float> synthBuffer;

    juce::dsp::Oscillator<float> osc { [](float x) { return std::sin(x); } };
    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};