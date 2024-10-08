#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EqualizerAudioProcessor::EqualizerAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

EqualizerAudioProcessor::~EqualizerAudioProcessor()
{
}

//==============================================================================
const juce::String EqualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EqualizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EqualizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EqualizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EqualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EqualizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EqualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EqualizerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String EqualizerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void EqualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void EqualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec {};
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare (spec);
    rightChain.prepare (spec);

    updateFilters();

    leftChannelFifo.prepare (samplesPerBlock);
    rightChannelFifo.prepare (samplesPerBlock);

    // osc.initialise ([](float x) {return std::sin (x); });
    // spec.numChannels = getTotalNumOutputChannels();
    // osc.prepare (spec);
    // osc.setFrequency(200);
}

void EqualizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool EqualizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void EqualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFilters();
    
    juce::dsp::AudioBlock<float> block { buffer };

    // buffer.clear();juce::dsp::ProcessContextReplacing<float> stereoContext (block);
    // osc.process (stereoContext);

    auto leftBlock { block.getSingleChannelBlock (0) };
    auto rightBlock { block.getSingleChannelBlock (1) };

    juce::dsp::ProcessContextReplacing<float> leftContext { leftBlock };
    juce::dsp::ProcessContextReplacing<float> rightContext { rightBlock };

    leftChain.process (leftContext);
    rightChain.process (rightContext);

    leftChannelFifo.update (buffer);
    rightChannelFifo.update (buffer);
}

//==============================================================================
bool EqualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EqualizerAudioProcessor::createEditor()
{
    return new EqualizerAudioProcessorEditor (*this);
}

//==============================================================================
void EqualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos { destData, true };
    apvts.state.writeToStream (mos);
}

void EqualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree { juce::ValueTree::readFromData (data, sizeInBytes) };
    if (tree.isValid())
    {
        apvts.replaceState (tree);
        updateFilters();
    }
}

ChainSettings getChainSettings (juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings {};

    settings.lowCutFreq = apvts.getRawParameterValue ("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue ("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue ("Peak Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue ("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue ("Peak Quality")->load();
    settings.lowCutSlope = static_cast<Slope> (apvts.getRawParameterValue ("LowCut Slope")->load());
    settings.highCutSlope = static_cast<Slope> (apvts.getRawParameterValue ("HighCut Slope")->load());

    return settings;
}

Coefficients makePeakFilter (const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter (sampleRate, 
                                                                chainSettings.peakFreq, 
                                                                chainSettings.peakQuality, 
                                                                juce::Decibels::decibelsToGain (chainSettings.peakGainInDecibels));
}

void EqualizerAudioProcessor::updatePeakFilter (const ChainSettings& chainSettings)
{
    auto peakCoefficients { makePeakFilter (chainSettings, getSampleRate()) };

    updateCoefficients (leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients (rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void EqualizerAudioProcessor::updateLowCutFilters (const ChainSettings& chainSettings)
{
    auto lowCutCoefficients { makeLowCutFilter (chainSettings, getSampleRate()) };

    auto& leftLowCut { leftChain.get<ChainPositions::LowCut>() };
    updateCutFilter (leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);

    auto& rightLowCut { rightChain.get<ChainPositions::LowCut>() };
    updateCutFilter (rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}

void EqualizerAudioProcessor::updateHighCutFilters (const ChainSettings& chainSettings)
{
    auto highCutCoefficients { makeHighCutFilter (chainSettings, getSampleRate()) };

    auto& leftHighCut { leftChain.get<ChainPositions::HighCut>() };
    updateCutFilter (leftHighCut, highCutCoefficients, chainSettings.highCutSlope);

    auto& rightHighCut { rightChain.get<ChainPositions::HighCut>() };
    updateCutFilter (rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void EqualizerAudioProcessor::updateFilters()
{
    auto chainSettings { getChainSettings (apvts) };

    updateLowCutFilters (chainSettings);
    updateHighCutFilters (chainSettings);
    updatePeakFilter (chainSettings);
}

juce::AudioProcessorValueTreeState::ParameterLayout EqualizerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat>("LowCut Freq", "LowCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.f));
    layout.add (std::make_unique<juce::AudioParameterFloat>("HighCut Freq", "HightCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f));
    layout.add (std::make_unique<juce::AudioParameterFloat>("Peak Freq", "Peak Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 750.f));
    layout.add (std::make_unique<juce::AudioParameterFloat>("Peak Gain", "Peak Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>("Peak Quality", "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f));

    juce::StringArray stringArray {};

    for (int i = 0; i < 4; ++i)
    {
        juce::String str {};
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add (str);
    }

    layout.add (std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EqualizerAudioProcessor();
}
