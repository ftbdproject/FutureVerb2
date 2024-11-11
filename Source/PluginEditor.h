// PluginEditor.h
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "Components/HeaderBar.h"
#include "Components/ContentArea.h"

class FutureVerb2AudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Timer
{
public:
    FutureVerb2AudioProcessorEditor(FutureVerb2AudioProcessor&);
    ~FutureVerb2AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    FutureVerb2AudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;

    juce::Typeface::Ptr regularTypeface;
    juce::Typeface::Ptr semiBoldTypeface;
    juce::Typeface::Ptr boldTypeface;

    juce::Image backgroundImage;

    std::unique_ptr<HeaderBar> headerBar;
    std::unique_ptr<ContentArea> contentArea;

    void initializeFonts();
    void initializeComponents();
    void setupCallbacks();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FutureVerb2AudioProcessorEditor)
};