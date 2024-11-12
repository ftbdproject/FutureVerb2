// VuMeter.h
#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

class VuMeter : public juce::Component,
    private juce::Timer
{
public:
    VuMeter();
    ~VuMeter() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void setValue(float newValue);

    // Customization options
    void setNumSegments(int segments);
    void setSegmentGap(float gapSize);
    void setDecibelsRange(float minDB, float maxDB);

private:
    void timerCallback() override;
    float dbToNormalized(float db) const;
    void drawDBMarkers(juce::Graphics& g, const juce::Rectangle<float>& bounds);

    float level = 0.0f;
    float targetLevel = 0.0f;

    // Meter configuration
    int numberOfSegments = 0;  // 0 for continuous mode
    float segmentGap = 1.0f;
    float minDecibels = -60.0f;
    float maxDecibels = 6.0f;

    // Colors
    const juce::Colour backgroundColour = juce::Colour(0xFF1B1B1B);
    const juce::Colour borderColour = juce::Colour(0xFF323232);
    const juce::Colour greenColour = juce::Colour(0xFF00FF00);
    const juce::Colour yellowColour = juce::Colour(0xFFFFFF00);
    const juce::Colour redColour = juce::Colour(0xFFFF0000);

    // Thresholds in dB
    const float yellowThresholdDb = -12.0f;
    const float redThresholdDb = -6.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VuMeter)
};
