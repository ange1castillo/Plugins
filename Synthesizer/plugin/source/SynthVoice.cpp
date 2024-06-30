#include "Synthesizer/SynthVoice.h"

SynthVoice::SynthVoice() {

}

SynthVoice::~SynthVoice() {

}

bool SynthVoice::canPlaySound (SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int pitchWheel) {
    osc.setFrequency (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    adsr.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff) {
    adsr.noteOff();
}

void SynthVoice::pitchWheelMoved (int newValue) {

}

void SynthVoice::controllerMoved (int controllerNumber, int newValue) {

}

void SynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert (isPrepared);

    juce::dsp::AudioBlock<float> audioBlock { outputBuffer };
    osc.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));

    adsr.applyEnvelopeToBuffer (outputBuffer, startSample, numSamples);
}

void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels) {

    adsr.setSampleRate (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    osc.prepare (spec);
    gain.prepare (spec);

    gain.setGainLinear (0.01f);

    isPrepared = true;
}