// FutureVerb2/Source/Core/ReverbProcessor.h
#pragma once
#include <JuceHeader.h>
#include <map>

class ReverbProcessor
{
public:
    ReverbProcessor();
    ~ReverbProcessor() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Parameter setters
    void setMidSideEnabled(bool enabled);
    void setHighpassFrequency(float freq);
    void setLowpassFrequency(float freq);
    void setModulation(float value);
    void setModulationSpeed(float value);
    void setBassMultiplier(float value);
    void setCrossfeedAmount(float value);
    void setERLevel(float value);
    void setStereoWidth(float value);
    void updateParameters(float size, float diffusion, float decay,
        float damping, float width, float preDelay, float mix = 0.5f);
    void setReverbType(const juce::String& type);

private:
    // Constants
    static constexpr float MAX_PREDELAY_SECONDS = 0.5f;
    static constexpr float SMOOTHING_TIME_SECONDS = 0.05f;

    // DSP Components
    juce::dsp::Reverb reverb;
    juce::Reverb::Parameters params;
    using FilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;
    FilterBand highpassFilter;
    FilterBand lowpassFilter;
    juce::AudioBuffer<float> preDelayBuffer;

    // State variables
    int preDelayWritePosition = 0;
    double currentSampleRate = 44100.0;
    float phasePosition = 0.0f;

    // Current parameter values
    float currentSize = 0.5f;
    float currentDiffusion = 0.5f;
    float currentDecay = 0.5f;
    float currentDamping = 0.5f;
    float currentWidth = 1.0f;
    float currentPreDelay = 0.0f;
    float currentMix = 0.5f;
    float currentModulation = 0.0f;
    float currentSpeed = 0.5f;
    float currentBass = 1.0f;
    float currentCross = 0.0f;
    float currentERLevel = 0.5f;
    float currentStereoSep = 1.0f;
    float highpassFreq = 20.0f;
    float lowpassFreq = 20000.0f;
    bool midSideEnabled = false;
    juce::String currentType = "HALL";

    // Parameter smoothing
    juce::LinearSmoothedValue<float> sizeSmoothed;
    juce::LinearSmoothedValue<float> diffusionSmoothed;
    juce::LinearSmoothedValue<float> decaySmoothed;
    juce::LinearSmoothedValue<float> dampingSmoothed;
    juce::LinearSmoothedValue<float> widthSmoothed;
    juce::LinearSmoothedValue<float> preDelaySmoothed;
    juce::LinearSmoothedValue<float> modulationSmoothed;
    juce::LinearSmoothedValue<float> speedSmoothed;
    juce::LinearSmoothedValue<float> bassSmoothed;
    juce::LinearSmoothedValue<float> crossSmoothed;

    // Initialization methods
    void initializeParameters();
    void initializeSmoothers();
    void initializeFilters();

    // Processing methods
    void updateFilters();
    void resetSmoothers(double sampleRate);
    void processPreDelay(juce::AudioBuffer<float>& buffer);
    void applyDiffusion(juce::AudioBuffer<float>& buffer);
    void processMidSide(juce::AudioBuffer<float>& buffer, bool encode);
    void applyModulation(juce::AudioBuffer<float>& buffer);
    void applyBassProcessing(juce::AudioBuffer<float>& buffer);
    void applyCrossfeed(juce::AudioBuffer<float>& buffer);
    void applyStereoSeparation(juce::AudioBuffer<float>& buffer);
    void applyMix(juce::AudioBuffer<float>& wetBuffer,
        const juce::AudioBuffer<float>& dryBuffer,
        float mix);
    void updateReverbParams();
    void applyReverbPreset(const juce::String& type);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};