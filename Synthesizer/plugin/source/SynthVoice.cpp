#include <JuceHeader.h>
#include "Synthesizer/SynthVoice.h"

SynthVoice::SynthVoice() {

}

SynthVoice::~SynthVoice() {

}

bool SynthVoice::canPlaySound (SynthesiserSound*) {

}

void SynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) {

}
void SynthVoice::stopNote (float velocity, bool allowTailOff) {

}

void SynthVoice::pitchWheelMoved (int newValue) {

}
void SynthVoice::controllerMoved (int controllerNumber, int newValue) {

}

void SynthVoice::renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) {

}