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

struct ResponseCurveComponent : juce::Component,
                                juce::AudioProcessorParameter::Listener,
                                juce::Timer
{
    ResponseCurveComponent (EqualizerAudioProcessor&);
    ~ResponseCurveComponent();

    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsString) override {};
    void timerCallback();
    void paint (juce::Graphics& g) override;
private:
    EqualizerAudioProcessor& processorRef;
    juce::Atomic<bool> parametersChanged { false };
    MonoChain monoChain {};
};

//==============================================================================
class EqualizerAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit EqualizerAudioProcessorEditor (EqualizerAudioProcessor&);
    ~EqualizerAudioProcessorEditor() override;

    //==============================================================================
    void resized() override;

private:
    EqualizerAudioProcessor& processorRef;

    CustomRotarySlider peakFreqSlider {};
    CustomRotarySlider peakGainSlider {};
    CustomRotarySlider peakQualitySlider {};
    CustomRotarySlider lowCutFreqSlider {};
    CustomRotarySlider highCutFreqSlider {};
    CustomRotarySlider lowCutSlopeSlider {};
    CustomRotarySlider highCutSlopeSlider {};

    ResponseCurveComponent responseCurveComponent;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerAudioProcessorEditor)
};
