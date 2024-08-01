#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                         juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

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

    CustomRotarySlider peakFreqSlider {};
    CustomRotarySlider peakGainSlider {};
    CustomRotarySlider peakQualitySlider {};
    CustomRotarySlider lowCutFreqSlider {};
    CustomRotarySlider highCutFreqSlider {};
    CustomRotarySlider lowCutSlopeSlider {};
    CustomRotarySlider highCutSlopeSlider {};

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment;
    Attachment peakGainSliderAttachment;
    Attachment peakQualitySliderAttachment;
    Attachment lowCutFreqSliderAttachment;
    Attachment highCutFreqSliderAttachment;
    Attachment lowCutSlopeSliderAttachment;
    Attachment highCutSlopeSliderAttachment;

    std::vector<juce::Component*> getComps();

    MonoChain monoChain {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerAudioProcessorEditor)
};
