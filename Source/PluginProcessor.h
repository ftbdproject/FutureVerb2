#pragma once

#include <JuceHeader.h>
#include "Core/ReverbProcessor.h"

class FutureVerb2AudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    FutureVerb2AudioProcessor();
    ~FutureVerb2AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void updateReverbParameters(float size, float diffusion, float decay,
        float damping, float width, float preDelay);
    void onParameterUpdate(const juce::String& paramId, float value);
    float getLeftLevel() const { return leftLevel.load(); }
    float getRightLevel() const { return rightLevel.load(); }

    // Output control methods
    void setLimiterEnabled(bool shouldLimit) { limiterEnabled = shouldLimit; }
    void setClipThreshold(float thresholdDb) { clipThresholdDb = thresholdDb; }
    float getOutputGainDb() const { return currentOutputGainDb; }

    juce::AudioProcessorValueTreeState& getValueTreeState() { return *parameters; }

private:
    std::unique_ptr<ReverbProcessor> reverbProcessor;
    std::unique_ptr<juce::AudioProcessorValueTreeState> parameters;
    double lastSampleRate;
    std::atomic<float> leftLevel{ 0.0f };
    std::atomic<float> rightLevel{ 0.0f };

    // Output control members
    juce::LinearSmoothedValue<float> smoothedOutputGain;
    float currentOutputGainDb{ 0.0f };
    float currentOutputGainLinear{ 1.0f };
    bool limiterEnabled{ true };
    float clipThresholdDb{ 0.0f };

    // Parameter IDs
    static constexpr const char* SIZE_ID = "size";
    static constexpr const char* DIFFUSION_ID = "diffusion";
    static constexpr const char* DECAY_ID = "decay";
    static constexpr const char* DAMPING_ID = "damping";
    static constexpr const char* WIDTH_ID = "width";
    static constexpr const char* PREDELAY_ID = "predelay";
    static constexpr const char* MIX_ID = "mix";
    static constexpr const char* MIDSIDE_ID = "midside";
    static constexpr const char* HIGHPASS_ID = "highpass";
    static constexpr const char* LOWPASS_ID = "lowpass";
    static constexpr const char* MODULATION_ID = "modulation";
    static constexpr const char* SPEED_ID = "speed";
    static constexpr const char* BASS_ID = "bass";
    static constexpr const char* CROSS_ID = "cross";
    static constexpr const char* ERLEVEL_ID = "erlevel";
    static constexpr const char* STEREOSEP_ID = "stereosep";
    static constexpr const char* TYPE_ID = "type";
    static constexpr const char* OUTPUT_ID = "output";

    void createParameterLayout();
    void setupParameterCallbacks();
    void processOutput(juce::AudioBuffer<float>& buffer);
    float softClip(float sample) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FutureVerb2AudioProcessor)
};