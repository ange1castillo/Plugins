#pragma once

#include <JuceHeader.h>
#include "Data/AdsrData.h"
#include "SynthSound.h"

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
    void update (const float attack, const float decay, const float sustain, const float release);

private:
    AdsrData adsr;
    juce::AudioBuffer<float> synthBuffer;

    juce::dsp::Oscillator<float> osc { [](float x) { return x / juce::MathConstants<float>::pi; } };
    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};