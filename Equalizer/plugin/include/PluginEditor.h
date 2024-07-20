#pragma once

#include "PluginProcessor.h"

//==============================================================================
class EqualizerAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit EqualizerAudioProcessorEditor (EqualizerAudioProcessor&);
    ~EqualizerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EqualizerAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerAudioProcessorEditor)
};
