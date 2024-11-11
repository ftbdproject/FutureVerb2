#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <array>

FutureVerb2AudioProcessor::FutureVerb2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    lastSampleRate(44100.0)
#endif
{
    reverbProcessor = std::make_unique<ReverbProcessor>();
    createParameterLayout();
}

FutureVerb2AudioProcessor::~FutureVerb2AudioProcessor()
{
    const std::array<const char*, 18> paramIDs = {
        SIZE_ID, DIFFUSION_ID, DECAY_ID, DAMPING_ID,
        WIDTH_ID, PREDELAY_ID, MIX_ID, TYPE_ID,
        MIDSIDE_ID, HIGHPASS_ID, LOWPASS_ID,
        MODULATION_ID, SPEED_ID, BASS_ID, CROSS_ID,
        ERLEVEL_ID, STEREOSEP_ID, OUTPUT_ID
    };

    for (auto paramID : paramIDs)
    {
        parameters->removeParameterListener(paramID, this);
    }

    parameters = nullptr;
    reverbProcessor = nullptr;
}

void FutureVerb2AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (!reverbProcessor) return;

    if (parameterID == MIDSIDE_ID)
        reverbProcessor->setMidSideEnabled(newValue > 0.5f);
    else if (parameterID == HIGHPASS_ID)
        reverbProcessor->setHighpassFrequency(newValue);
    else if (parameterID == LOWPASS_ID)
        reverbProcessor->setLowpassFrequency(newValue);
    else if (parameterID == MODULATION_ID)
        reverbProcessor->setModulation(newValue);
    else if (parameterID == SPEED_ID)
        reverbProcessor->setModulationSpeed(newValue);
    else if (parameterID == BASS_ID)
        reverbProcessor->setBassMultiplier(newValue);
    else if (parameterID == CROSS_ID)
        reverbProcessor->setCrossfeedAmount(newValue);
    else if (parameterID == ERLEVEL_ID)
        reverbProcessor->setERLevel(newValue);
    else if (parameterID == STEREOSEP_ID)
        reverbProcessor->setStereoWidth(newValue);
    else if (parameterID == TYPE_ID)
    {
        int typeIndex = static_cast<int>(newValue);
        const juce::StringArray types = { "ROOM", "PLATE", "HALL", "CHAMBER", "SPRING" };
        if (typeIndex >= 0 && typeIndex < types.size())
        {
            reverbProcessor->setReverbType(types[typeIndex]);
        }
    }
    else if (parameterID == OUTPUT_ID)
    {
        currentOutputGainDb = newValue;
        currentOutputGainLinear = juce::Decibels::decibelsToGain(newValue);
        smoothedOutputGain.setTargetValue(currentOutputGainLinear);
    }

    // Update main reverb parameters
    float size = parameters->getRawParameterValue(SIZE_ID)->load();
    float diffusion = parameters->getRawParameterValue(DIFFUSION_ID)->load();
    float decay = parameters->getRawParameterValue(DECAY_ID)->load();
    float damping = parameters->getRawParameterValue(DAMPING_ID)->load();
    float width = parameters->getRawParameterValue(WIDTH_ID)->load();
    float preDelay = parameters->getRawParameterValue(PREDELAY_ID)->load() / 1000.0f;
    float mix = parameters->getRawParameterValue(MIX_ID)->load();

    reverbProcessor->updateParameters(size, diffusion, decay, damping, width, preDelay, mix);
}

void FutureVerb2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;

    // Initialize output gain smoother
    smoothedOutputGain.reset(sampleRate, 0.05); // 50ms smoothing time
    smoothedOutputGain.setCurrentAndTargetValue(currentOutputGainLinear);

    if (reverbProcessor)
    {
        reverbProcessor->prepare(sampleRate, samplesPerBlock);
    }
}


void FutureVerb2AudioProcessor::updateReverbParameters(float size, float diffusion,
    float decay, float damping, float width, float preDelay)
{
    if (reverbProcessor)
    {
        float mix = parameters->getRawParameterValue(MIX_ID)->load();
        float output = parameters->getRawParameterValue(OUTPUT_ID)->load();
        reverbProcessor->updateParameters(size, diffusion, decay,
            damping, width, preDelay, mix);
        // Handle output separately if needed
    }
}

void FutureVerb2AudioProcessor::releaseResources()
{
    if (reverbProcessor)
    {
        reverbProcessor->reset();
    }
}

void FutureVerb2AudioProcessor::onParameterUpdate(const juce::String& paramId, float value)
{
    if (!reverbProcessor) return;

    if (paramId == MIDSIDE_ID)
        reverbProcessor->setMidSideEnabled(value > 0.5f);
    else if (paramId == HIGHPASS_ID)
        reverbProcessor->setHighpassFrequency(value);
    else if (paramId == LOWPASS_ID)
        reverbProcessor->setLowpassFrequency(value);
    else if (paramId == MODULATION_ID)
        reverbProcessor->setModulation(value);
    else if (paramId == SPEED_ID)
        reverbProcessor->setModulationSpeed(value);
    else if (paramId == BASS_ID)
        reverbProcessor->setBassMultiplier(value);
    else if (paramId == CROSS_ID)
        reverbProcessor->setCrossfeedAmount(value);
    else if (paramId == ERLEVEL_ID)
        reverbProcessor->setERLevel(value);
    else if (paramId == STEREOSEP_ID)
        reverbProcessor->setStereoWidth(value);
}

bool FutureVerb2AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    const auto mainOutput = layouts.getMainOutputChannelSet();
    const auto mainInput = layouts.getMainInputChannelSet();

    const bool isOutputValid = (mainOutput == juce::AudioChannelSet::mono())
        || (mainOutput == juce::AudioChannelSet::stereo());

    if (!isOutputValid)
        return false;

#if ! JucePlugin_IsSynth
    return mainInput == mainOutput;
#else
    return true;
#endif
#endif
}

void FutureVerb2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (buffer.getNumChannels() > 0)
        leftLevel.store(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (buffer.getNumChannels() > 1)
        rightLevel.store(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

    if (reverbProcessor)
        reverbProcessor->process(buffer);

    processOutput(buffer);
}

void FutureVerb2AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters->copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);
}

void FutureVerb2AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters->state.getType()))
            parameters->replaceState(juce::ValueTree::fromXml(*xmlState));
}

void FutureVerb2AudioProcessor::reset()
{
    if (reverbProcessor)
        reverbProcessor->reset();

    // Reset output control
    currentOutputGainDb = 0.0f;
    currentOutputGainLinear = 1.0f;
    smoothedOutputGain.setCurrentAndTargetValue(currentOutputGainLinear);
}
void FutureVerb2AudioProcessor::processOutput(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (smoothedOutputGain.isSmoothing())
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                const float gainValue = smoothedOutputGain.getNextValue();

                if (limiterEnabled)
                    channelData[sample] = softClip(channelData[sample] * gainValue);
                else
                    channelData[sample] *= gainValue;
            }
        }
    }
    else
    {
        if (limiterEnabled)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);

                for (int sample = 0; sample < numSamples; ++sample)
                {
                    channelData[sample] = softClip(channelData[sample] * currentOutputGainLinear);
                }
            }
        }
        else
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                buffer.applyGain(channel, 0, numSamples, currentOutputGainLinear);
            }
        }
    }
}


float FutureVerb2AudioProcessor::softClip(float sample) const
{
    const float threshold = juce::Decibels::decibelsToGain(clipThresholdDb);
    if (std::abs(sample) > threshold)
    {
        return threshold * std::tanh(sample / threshold);
    }
    return sample;
}

void FutureVerb2AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterBool>(
        MIDSIDE_ID, "Mid/Side", false));

    // More conservative filter ranges
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        HIGHPASS_ID, "Highpass",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.3f), // Maximum 2kHz
        20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        LOWPASS_ID, "Lowpass",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.3f), // Minimum 200Hz
        20000.0f));

    // Safer ranges for other potentially problematic parameters
    auto sizeRange = juce::NormalisableRange<float>(0.0f, 1.0f);
    sizeRange.setSkewForCentre(0.3f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        SIZE_ID, "Size", sizeRange, 0.5f));

    auto diffusionRange = juce::NormalisableRange<float>(0.0f, 0.95f); // Limit maximum diffusion
    diffusionRange.setSkewForCentre(0.5f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        DIFFUSION_ID, "Diffusion", diffusionRange, 0.5f));

    auto decayRange = juce::NormalisableRange<float>(0.1f, 20.0f, 0.1f);
    decayRange.setSkewForCentre(2.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        DECAY_ID, "Decay", decayRange, 2.0f));

    auto dampingRange = juce::NormalisableRange<float>(0.0f, 1.0f);
    dampingRange.setSkewForCentre(0.5f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        DAMPING_ID, "Damping", dampingRange, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        WIDTH_ID, "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        MODULATION_ID, "Modulation",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        SPEED_ID, "Speed",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        BASS_ID, "Bass",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        CROSS_ID, "Cross",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // More conservative pre-delay range
    auto preDelayRange = juce::NormalisableRange<float>(0.0f, 250.0f, 1.0f); // Maximum 250ms
    preDelayRange.setSkewForCentre(50.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PREDELAY_ID, "Pre-Delay", preDelayRange, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ TYPE_ID, 1 },  // Add version number
        "Type",
        juce::StringArray{ "ROOM", "PLATE", "HALL", "CHAMBER", "SPRING" },
        2));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        MIX_ID, "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ERLEVEL_ID, "ER Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        STEREOSEP_ID, "Stereo Separation",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // Safer output range
    auto outputRange = juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f); // Maximum +6dB instead of +12dB
    outputRange.setSkewForCentre(0.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        OUTPUT_ID, "Output",
        outputRange,
        0.0f));

    parameters = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, nullptr, "Parameters", std::move(layout));

    setupParameterCallbacks();
}

void FutureVerb2AudioProcessor::setupParameterCallbacks()
{
    const std::array<const char*, 18> paramIDs = {
        SIZE_ID, DIFFUSION_ID, DECAY_ID, DAMPING_ID,
        WIDTH_ID, PREDELAY_ID, MIX_ID, TYPE_ID,
        MIDSIDE_ID, HIGHPASS_ID, LOWPASS_ID,
        MODULATION_ID, SPEED_ID, BASS_ID, CROSS_ID,
        ERLEVEL_ID, STEREOSEP_ID, OUTPUT_ID
    };

    for (auto paramID : paramIDs)
    {
        parameters->addParameterListener(paramID, this);
    }
}

bool FutureVerb2AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FutureVerb2AudioProcessor::createEditor() { return new FutureVerb2AudioProcessorEditor(*this); }
const juce::String FutureVerb2AudioProcessor::getName() const { return JucePlugin_Name; }
bool FutureVerb2AudioProcessor::acceptsMidi() const { return false; }
bool FutureVerb2AudioProcessor::producesMidi() const { return false; }
bool FutureVerb2AudioProcessor::isMidiEffect() const { return false; }
double FutureVerb2AudioProcessor::getTailLengthSeconds() const { return 5.0; }
int FutureVerb2AudioProcessor::getNumPrograms() { return 1; }
int FutureVerb2AudioProcessor::getCurrentProgram() { return 0; }
void FutureVerb2AudioProcessor::setCurrentProgram(int index) {}
const juce::String FutureVerb2AudioProcessor::getProgramName(int index) { return {}; }
void FutureVerb2AudioProcessor::changeProgramName(int index, const juce::String& newName) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FutureVerb2AudioProcessor();
}
