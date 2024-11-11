// VuMeter.cpp
#include "VuMeter.h"

VuMeter::VuMeter()
{
    setOpaque(true);
    startTimerHz(60);
}

VuMeter::~VuMeter()
{
    stopTimer();
}

void VuMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    // Background
    g.setColour(backgroundColour);
    g.fillRect(bounds);

    // Draw meter
    auto meterBounds = bounds.reduced(2.0f);
    float meterHeight = meterBounds.getHeight() * level;
    auto activeMeterBounds = meterBounds.removeFromBottom(meterHeight);

    // Draw background grid lines
    g.setColour(borderColour.brighter(0.1f));
    float dbMarkers[] = { 0.0f, -3.0f, -6.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f };
    for (float db : dbMarkers)
    {
        float y = meterBounds.getY() + (1.0f - dbToNormalized(db)) * meterBounds.getHeight();
        g.drawHorizontalLine(static_cast<int>(y), meterBounds.getX(), meterBounds.getRight());
    }

    // Draw meter gradient
    if (activeMeterBounds.getHeight() > 0)
    {
        juce::ColourGradient gradient(
            greenColour,
            activeMeterBounds.getBottomLeft(),
            redColour,
            activeMeterBounds.getTopLeft(),
            false
        );

        gradient.addColour(dbToNormalized(yellowThresholdDb), yellowColour);
        gradient.addColour(dbToNormalized(redThresholdDb), redColour);

        g.setGradientFill(gradient);
        g.fillRect(activeMeterBounds);
    }

    // Draw border
    g.setColour(borderColour);
    g.drawRect(bounds, 1.0f);

    // Draw dB markers
    drawDBMarkers(g, bounds);
}

void VuMeter::drawDBMarkers(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(10.0f);

    float dbMarkers[] = { 0.0f, -3.0f, -6.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f };
    for (float db : dbMarkers)
    {
        float y = bounds.getY() + (1.0f - dbToNormalized(db)) * bounds.getHeight();
        juce::String text = juce::String(static_cast<int>(db));
        g.drawText(text, bounds.getRight() + 2.0f, y - 5.0f, 20.0f, 10.0f, juce::Justification::left);
    }
}

float VuMeter::dbToNormalized(float db) const
{
    return juce::jlimit(0.0f, 1.0f,
        (db - minDecibels) / (maxDecibels - minDecibels));
}

void VuMeter::resized()
{
}

void VuMeter::setValue(float newValue)
{
    // Convert from normalized value to dB for smoother meter movement
    float dbValue = juce::Decibels::gainToDecibels(newValue, minDecibels);
    targetLevel = dbToNormalized(dbValue);
}

void VuMeter::timerCallback()
{
    const float smoothingFactor = 0.2f;
    float diff = targetLevel - level;

    if (std::abs(diff) > 0.001f)
    {
        level += diff * smoothingFactor;
        repaint();
    }
}

void VuMeter::setNumSegments(int segments)
{
    numberOfSegments = segments;
    repaint();
}

void VuMeter::setSegmentGap(float gapSize)
{
    segmentGap = gapSize;
    repaint();
}

void VuMeter::setDecibelsRange(float minDB, float maxDB)
{
    minDecibels = minDB;
    maxDecibels = maxDB;
    repaint();
}