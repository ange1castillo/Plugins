#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <JuceHeader.h>

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float peakFreq { 0 };
    float peakGainInDecibels { 0 };
    float peakQuality { 1.f };
    float lowCutFreq { 0 };
    float highCutFreq { 0 };
    Slope lowCutSlope { Slope::Slope_12 };
    Slope highCutSlope { Slope::Slope_12 };
};

ChainSettings getChainSettings (juce::AudioProcessorValueTreeState& apvts);

using Filter = juce::dsp::IIR::Filter<float>;

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions
{
    LowCut,
    Peak,
    HighCut,
    maxChainPositions
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients (Coefficients& old, const Coefficients& replacement);

Coefficients makePeakFilter (const ChainSettings& chainSettings, double sampleRate);

template <int Index, typename ChainType, typename CoefficientType>
void update (ChainType& chain, const CoefficientType& coefficients)
{
    updateCoefficients (chain.template get<Index>().coefficients, coefficients[Index]);
    chain.template setBypassed<Index> (false);
}

template <typename ChainType, typename CoefficientType>
void updateCutFilter (ChainType& leftLowCut, const CoefficientType& cutCoefficients, const Slope& lowCutSlope)
{
    leftLowCut.template setBypassed<0> (true);
    leftLowCut.template setBypassed<1> (true);
    leftLowCut.template setBypassed<2> (true);
    leftLowCut.template setBypassed<3> (true);

    switch (lowCutSlope)
    {
        case Slope_48:
        {
            update<3> (leftLowCut, cutCoefficients);
        }

        case Slope_36:
        {
            update<2> (leftLowCut, cutCoefficients);
        }

        case Slope_24:
        {
            update<1> (leftLowCut, cutCoefficients);
        }

        case Slope_12:
        {
            update<0> (leftLowCut, cutCoefficients);
        }
    }
}

inline auto makeLowCutFilter (const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod (chainSettings.lowCutFreq, sampleRate, 2 * (chainSettings.lowCutSlope + 1));
}

inline auto makeHighCutFilter (const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod (chainSettings.highCutFreq, sampleRate, 2 * (chainSettings.highCutSlope + 1));
}

//==============================================================================
class EqualizerAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    EqualizerAudioProcessor();
    ~EqualizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };    

private:
    MonoChain leftChain;
    MonoChain rightChain;

    void updatePeakFilter (const ChainSettings& chainSettings);
    void updateLowCutFilters (const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    void updateFilters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerAudioProcessor)
};
