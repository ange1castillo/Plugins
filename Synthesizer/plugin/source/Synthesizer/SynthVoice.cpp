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

    if(!allowTailOff || !adsr.isActive()) {
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved (int newValue) {

}

void SynthVoice::controllerMoved (int controllerNumber, int newValue) {

}

void SynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert (isPrepared);

    if (!isVoiceActive()) {
        return;
    }

    synthBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    juce::dsp::AudioBlock<float> audioBlock { synthBuffer };
    osc.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));

    adsr.applyEnvelopeToBuffer (synthBuffer, 0, synthBuffer.getNumSamples());

    for(int channel { 0 }; channel < outputBuffer.getNumChannels(); ++channel) {
        outputBuffer.addFrom (channel, startSample, synthBuffer, channel, 0, numSamples);

        if(!adsr.isActive()) {
            clearCurrentNote();
        }
    }
}

void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels) {

    adsr.setSampleRate (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    osc.prepare (spec);
    gain.prepare (spec);

    gain.setGainLinear (0.03f);

    isPrepared = true;
}

void SynthVoice::updateADSR (const float attack, const float decay, const float sustain, const float release) {
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    adsr.setParameters (adsrParams);
}