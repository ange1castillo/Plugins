#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EqualizerAudioProcessorEditor::EqualizerAudioProcessorEditor (EqualizerAudioProcessor& p)
    : AudioProcessorEditor (&p), 
    processorRef (p), 
    peakFreqSliderAttachment (processorRef.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment (processorRef.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment (processorRef.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachment (processorRef.apvts, "LowCut Freq", lowCutFreqSlider),
    highCutFreqSliderAttachment (processorRef.apvts, "HighCut Freq", highCutFreqSlider),
    lowCutSlopeSliderAttachment (processorRef.apvts, "LowCut Slope", lowCutSlopeSlider),
    highCutSlopeSliderAttachment (processorRef.apvts, "HighCut Slope", highCutSlopeSlider)
{
    for (auto* comp : getComps())
    {
        addAndMakeVisible (comp);
    }
    setSize (600, 400);
}

EqualizerAudioProcessorEditor::~EqualizerAudioProcessorEditor()
{
}

//==============================================================================
void EqualizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    auto bounds { getLocalBounds() };
    auto responseArea { bounds.removeFromTop (bounds.getHeight() * 0.33) };
    auto width { responseArea.getWidth() };

    auto& lowcut { monoChain.get<ChainPositions::LowCut>() };
    auto& peak { monoChain.get<ChainPositions::Peak>() };
    auto& highcut { monoChain.get<ChainPositions::HighCut>() };

    auto sampleRate { processorRef.getSampleRate() };

    std::vector<double> mags {};

    mags.resize (width);

    for (int i { 0 }; i < width; ++i)
    {
        double mag { 1.f };
        auto freq { mapToLog10 (double (i) / double (width), 20.0, 20000.0) };

        if (!monoChain.isBypassed<ChainPositions::Peak>())
        {
            mag *= peak.coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!lowcut.isBypassed<0>())
        {
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!lowcut.isBypassed<1>())
        {
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!lowcut.isBypassed<2>())
        {
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!lowcut.isBypassed<3>())
        {
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!highcut.isBypassed<0>())
        {
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!highcut.isBypassed<1>())
        {
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!highcut.isBypassed<2>())
        {
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        if (!highcut.isBypassed<3>())
        {
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency (freq, sampleRate);
        }

        mags[i] = Decibels::gainToDecibels (mag);
    }

    Path responseCurve {};

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return jmap (input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath (responseArea.getX(), map (mags.front()));

    for (size_t i { 1 }; i < mags.size(); ++i)
    {
        responseCurve.lineTo (responseArea.getX() + i, map (mags[i]));
    }

    g.setColour (Colours::orange);
    g.drawRoundedRectangle (responseArea.toFloat(), 4.f, 1.f);

    g.setColour (Colours::white);
    g.strokePath (responseCurve, PathStrokeType (2.f));
}

void EqualizerAudioProcessorEditor::resized()
{
    auto bounds { getLocalBounds() };
    auto responsiveArea { bounds.removeFromTop (bounds.getHeight() * 0.33) };
    auto lowCutArea { bounds.removeFromLeft (bounds.getWidth() * 0.33) };
    auto highCutArea { bounds.removeFromRight (bounds.getWidth() * 0.5) };

    lowCutFreqSlider.setBounds (lowCutArea.removeFromTop (lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds (lowCutArea);

    highCutFreqSlider.setBounds (highCutArea.removeFromTop (highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds (highCutArea);

    peakFreqSlider.setBounds (bounds.removeFromTop (bounds.getHeight() * 0.33));
    peakGainSlider.setBounds (bounds.removeFromTop (bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds (bounds);
}

void EqualizerAudioProcessorEditor::parameterValueChanged (int parameterIndex, float newValue)
{
    parametersChanged.set (true);
}

void EqualizerAudioProcessorEditor::timerCallback()
{
    if (parametersChanged.compareAndSetBool (false, true))
    {
        
    }
}

std::vector<juce::Component*> EqualizerAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider
    };
}