#pragma once

#include <JuceHeader.h>

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
};