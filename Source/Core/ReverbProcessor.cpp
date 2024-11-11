/*
  ==============================================================================

    ReverbProcessor.cpp
    Created: 11 Nov 2024 9:18:57am
    Author:  KinoBeddiez

  ==============================================================================
*/

// FutureVerb2/Source/Core/ReverbProcessor.cpp

#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor()
{
    initializeParameters();
    initializeSmoothers();
    initializeFilters();
}

void ReverbProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    reverb.prepare(spec);
    highpassFilter.prepare(spec);
    lowpassFilter.prepare(spec);

    resetSmoothers(sampleRate);
    preDelayBuffer.setSize(2, static_cast<int>(sampleRate * MAX_PREDELAY_SECONDS));
    preDelayBuffer.clear();
    preDelayWritePosition = 0;

    updateReverbParams();
}

void ReverbProcessor::reset()
{
    preDelayBuffer.clear();
    preDelayWritePosition = 0;
    reverb.reset();
    highpassFilter.reset();
    lowpassFilter.reset();
    resetSmoothers(currentSampleRate);

    sizeSmoothed.setCurrentAndTargetValue(currentSize);
    diffusionSmoothed.setCurrentAndTargetValue(currentDiffusion);
    decaySmoothed.setCurrentAndTargetValue(currentDecay);
    dampingSmoothed.setCurrentAndTargetValue(currentDamping);
    widthSmoothed.setCurrentAndTargetValue(currentWidth);
    preDelaySmoothed.setCurrentAndTargetValue(currentPreDelay);
    modulationSmoothed.setCurrentAndTargetValue(currentModulation);
    speedSmoothed.setCurrentAndTargetValue(currentSpeed);
    bassSmoothed.setCurrentAndTargetValue(currentBass);
    crossSmoothed.setCurrentAndTargetValue(currentCross);
}

void ReverbProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    if (midSideEnabled)
        processMidSide(buffer, true);

    highpassFilter.process(context);
    lowpassFilter.process(context);

    if (currentPreDelay > 0.0f)
        processPreDelay(buffer);

    if (currentDiffusion > 0.0f)
        applyDiffusion(buffer);

    if (currentModulation > 0.0f)
        applyModulation(buffer);

    reverb.process(context);

    if (currentBass != 1.0f)
        applyBassProcessing(buffer);

    if (currentCross > 0.0f)
        applyCrossfeed(buffer);

    if (currentStereoSep != 1.0f)
        applyStereoSeparation(buffer);

    if (midSideEnabled)
        processMidSide(buffer, false);

    applyMix(buffer, dryBuffer, currentMix);
}

void ReverbProcessor::initializeParameters()
{
    params.roomSize = 0.5f;
    params.damping = 0.5f;
    params.wetLevel = 0.33f;
    params.dryLevel = 0.4f;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
}

void ReverbProcessor::initializeSmoothers()
{
    sizeSmoothed.reset(50);
    diffusionSmoothed.reset(50);
    decaySmoothed.reset(50);
    dampingSmoothed.reset(50);
    widthSmoothed.reset(50);
    preDelaySmoothed.reset(50);
    modulationSmoothed.reset(50);
    speedSmoothed.reset(50);
    bassSmoothed.reset(50);
    crossSmoothed.reset(50);
}

void ReverbProcessor::initializeFilters()
{
    auto highpassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, highpassFreq);
    auto lowpassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, lowpassFreq);

    *highpassFilter.state = *highpassCoeffs;
    *lowpassFilter.state = *lowpassCoeffs;
}

void ReverbProcessor::updateFilters()
{
    auto highpassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, highpassFreq);
    auto lowpassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, lowpassFreq);

    *highpassFilter.state = *highpassCoeffs;
    *lowpassFilter.state = *lowpassCoeffs;
}

void ReverbProcessor::setMidSideEnabled(bool enabled)
{
    midSideEnabled = enabled;
}

void ReverbProcessor::setHighpassFrequency(float freq)
{
    highpassFreq = freq;
    updateFilters();
}

void ReverbProcessor::setLowpassFrequency(float freq)
{
    lowpassFreq = freq;
    updateFilters();
}

void ReverbProcessor::setModulation(float value)
{
    currentModulation = value;
    modulationSmoothed.setTargetValue(value);
}

void ReverbProcessor::setModulationSpeed(float value)
{
    currentSpeed = value;
    speedSmoothed.setTargetValue(value);
}

void ReverbProcessor::setBassMultiplier(float value)
{
    currentBass = value;
    bassSmoothed.setTargetValue(value);
}

void ReverbProcessor::setCrossfeedAmount(float value)
{
    currentCross = value;
    crossSmoothed.setTargetValue(value);
}

void ReverbProcessor::setERLevel(float value)
{
    currentERLevel = value;
}

void ReverbProcessor::setStereoWidth(float value)
{
    currentStereoSep = value;
}

void ReverbProcessor::updateParameters(float size, float diffusion, float decay,
    float damping, float width, float preDelay, float mix)
{
    sizeSmoothed.setTargetValue(size);
    diffusionSmoothed.setTargetValue(diffusion);
    decaySmoothed.setTargetValue(decay);
    dampingSmoothed.setTargetValue(damping);
    widthSmoothed.setTargetValue(width);
    preDelaySmoothed.setTargetValue(preDelay);

    currentSize = size;
    currentDiffusion = diffusion;
    currentDecay = decay;
    currentDamping = damping;
    currentWidth = width;
    currentPreDelay = preDelay;
    currentMix = mix;

    updateReverbParams();
}

void ReverbProcessor::setReverbType(const juce::String& type)
{
    currentType = type;
    applyReverbPreset(type);
    updateReverbParams();
}

void ReverbProcessor::resetSmoothers(double sampleRate)
{
    auto smoothTime = SMOOTHING_TIME_SECONDS;
    sizeSmoothed.reset(sampleRate, smoothTime);
    diffusionSmoothed.reset(sampleRate, smoothTime);
    decaySmoothed.reset(sampleRate, smoothTime);
    dampingSmoothed.reset(sampleRate, smoothTime);
    widthSmoothed.reset(sampleRate, smoothTime);
    preDelaySmoothed.reset(sampleRate, smoothTime);
    modulationSmoothed.reset(sampleRate, smoothTime);
    speedSmoothed.reset(sampleRate, smoothTime);
    bassSmoothed.reset(sampleRate, smoothTime);
    crossSmoothed.reset(sampleRate, smoothTime);
}

void ReverbProcessor::processPreDelay(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    const int delayInSamples = static_cast<int>(currentPreDelay * currentSampleRate);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = preDelayBuffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float in = channelData[sample];
            int readPosition = preDelayWritePosition - delayInSamples;
            if (readPosition < 0) readPosition += preDelayBuffer.getNumSamples();
            delayData[preDelayWritePosition] = in;
            channelData[sample] = delayData[readPosition];
            if (++preDelayWritePosition >= preDelayBuffer.getNumSamples())
                preDelayWritePosition = 0;
        }
    }
}

void ReverbProcessor::applyDiffusion(juce::AudioBuffer<float>& buffer)
{
    const float diffusionAmount = currentDiffusion;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 1; sample < buffer.getNumSamples(); ++sample)
        {
            float current = channelData[sample];
            float previous = channelData[sample - 1];
            channelData[sample] = current + (previous - current) * diffusionAmount;
        }
    }
}

void ReverbProcessor::processMidSide(juce::AudioBuffer<float>& buffer, bool encode)
{
    if (buffer.getNumChannels() != 2) return;

    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float l = left[i];
        float r = right[i];

        if (encode)
        {
            left[i] = (l + r) * 0.5f;
            right[i] = (l - r) * 0.5f;
        }
        else
        {
            left[i] = l + r;
            right[i] = l - r;
        }
    }
}

void ReverbProcessor::applyModulation(juce::AudioBuffer<float>& buffer)
{
    const float modulationAmount = modulationSmoothed.getNextValue();
    const float speed = speedSmoothed.getNextValue() * 5.0f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* data = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float lfo = std::sin(2.0f * juce::MathConstants<float>::pi * phasePosition);
            data[sample] *= 1.0f + (lfo * modulationAmount * 0.2f);

            phasePosition += speed / currentSampleRate;
            if (phasePosition >= 1.0f)
                phasePosition -= 1.0f;
        }
    }
}

void ReverbProcessor::applyBassProcessing(juce::AudioBuffer<float>& buffer)
{
    const float bassMultiplier = bassSmoothed.getNextValue();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* data = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            data[sample] *= bassMultiplier;
        }
    }
}

void ReverbProcessor::applyCrossfeed(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() != 2) return;

    const float crossAmount = crossSmoothed.getNextValue();
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float l = left[sample];
        float r = right[sample];

        left[sample] = l + (r * crossAmount);
        right[sample] = r + (l * crossAmount);
    }
}

void ReverbProcessor::applyStereoSeparation(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() != 2) return;

    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float mid = (left[sample] + right[sample]) * 0.5f;
        float side = (left[sample] - right[sample]) * 0.5f;

        side *= currentStereoSep;

        left[sample] = mid + side;
        right[sample] = mid - side;
    }
}

void ReverbProcessor::applyMix(juce::AudioBuffer<float>& wetBuffer,
    const juce::AudioBuffer<float>& dryBuffer, float mix)
{
    const float wetGain = mix;
    const float dryGain = 1.0f - mix;

    for (int channel = 0; channel < wetBuffer.getNumChannels(); ++channel)
    {
        float* wet = wetBuffer.getWritePointer(channel);
        const float* dry = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < wetBuffer.getNumSamples(); ++sample)
        {
            wet[sample] = wet[sample] * wetGain + dry[sample] * dryGain;
        }
    }
}

void ReverbProcessor::updateReverbParams()
{
    params.roomSize = currentSize;
    params.damping = currentDamping;
    params.wetLevel = currentDecay;
    params.width = currentWidth;
    reverb.setParameters(params);
}

void ReverbProcessor::applyReverbPreset(const juce::String& type)
{
    struct ReverbPreset {
        float size, damping, wet, dry, width, diffusion;
    };

    const std::map<juce::String, ReverbPreset> presets = {
        {"ROOM",    {0.3f, 0.5f, 0.3f, 0.7f, 0.8f, 0.4f}},
        {"PLATE",   {0.45f, 0.3f, 0.35f, 0.65f, 1.0f, 0.7f}},
        {"HALL",    {0.8f, 0.3f, 0.4f, 0.6f, 1.0f, 0.6f}},
        {"CHAMBER", {0.5f, 0.4f, 0.35f, 0.65f, 0.9f, 0.5f}},
        {"SPRING",  {0.2f, 0.7f, 0.3f, 0.7f, 0.5f, 0.8f}}
    };

    if (auto preset = presets.find(type); preset != presets.end())
    {
        const auto& p = preset->second;
        params.roomSize = p.size;
        params.damping = p.damping;
        params.wetLevel = p.wet;
        params.dryLevel = p.dry;
        params.width = p.width;
        currentDiffusion = p.diffusion;
    }
}