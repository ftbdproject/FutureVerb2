// PluginEditor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

FutureVerb2AudioProcessorEditor::FutureVerb2AudioProcessorEditor(FutureVerb2AudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
{
    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::interfacex2_png,      // Changed from background_svg
        BinaryData::interfacex2_pngSize   // Changed from background_svgSize
    );
    initializeFonts();
    initializeComponents();
    setupCallbacks();
    startTimerHz(30); // For VU meter updates
    setSize(1000, 380);
}

FutureVerb2AudioProcessorEditor::~FutureVerb2AudioProcessorEditor()
{
    stopTimer();
    contentArea = nullptr;
    headerBar = nullptr;
    setLookAndFeel(nullptr);
}

void FutureVerb2AudioProcessorEditor::initializeFonts()
{
    regularTypeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::IBMPlexSansRegular_ttf,
        BinaryData::IBMPlexSansRegular_ttfSize
    );

    semiBoldTypeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::IBMPlexSansSemiBold_ttf,
        BinaryData::IBMPlexSansSemiBold_ttfSize
    );

    boldTypeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::IBMPlexSansBold_ttf,
        BinaryData::IBMPlexSansBold_ttfSize
    );
}

void FutureVerb2AudioProcessorEditor::initializeComponents()
{
    setLookAndFeel(&customLookAndFeel);
    setOpaque(true);

    headerBar = std::make_unique<HeaderBar>(audioProcessor.getValueTreeState());
    if (headerBar != nullptr)
    {
        addAndMakeVisible(headerBar.get());
    }

    contentArea = std::make_unique<ContentArea>(audioProcessor.getValueTreeState());
    if (contentArea != nullptr)
    {
        addAndMakeVisible(contentArea.get());
    }
}

void FutureVerb2AudioProcessorEditor::setupCallbacks()
{
    // Future callback setup
}

void FutureVerb2AudioProcessorEditor::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
    }
    else
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }
}

void FutureVerb2AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    const int headerHeight = 40;
    if (headerBar != nullptr)
    {
        headerBar->setBounds(area.removeFromTop(headerHeight));
    }

    if (contentArea != nullptr)
    {
        contentArea->setBounds(area);
    }
}

void FutureVerb2AudioProcessorEditor::timerCallback()
{
    if (contentArea != nullptr)
    {
        // Get RMS levels from processor
        float leftRMS = audioProcessor.getLeftLevel();
        float rightRMS = audioProcessor.getRightLevel();

        // Convert to decibels
        float leftDb = juce::Decibels::gainToDecibels(leftRMS, -60.0f);
        float rightDb = juce::Decibels::gainToDecibels(rightRMS, -60.0f);

        // Normalize to 0-1 range
        float leftNormalized = juce::jmap(leftDb, -60.0f, 0.0f, 0.0f, 1.0f);
        float rightNormalized = juce::jmap(rightDb, -60.0f, 0.0f, 0.0f, 1.0f);

        // Clamp values between 0 and 1
        leftNormalized = juce::jlimit(0.0f, 1.0f, leftNormalized);
        rightNormalized = juce::jlimit(0.0f, 1.0f, rightNormalized);

        contentArea->setVuMeterLevels(leftNormalized, rightNormalized);
    }
}