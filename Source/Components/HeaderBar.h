// HeaderBar.h
#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

class ReverbButton : public juce::Button
{
public:
    ReverbButton(const juce::String& name, juce::Drawable* drawable)
        : Button(name), icon(drawable)
    {
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds();

        // Get text width
        g.setFont(12.0f);
        const float textWidth = g.getCurrentFont().getStringWidth(getButtonText());

        // Calculate total content width (icon + spacing + text)
        const float iconSize = 16.0f;  // Fixed icon size
        const float spacing = 6.0f;    // Space between icon and text
        const float totalWidth = iconSize + spacing + textWidth;

        // Center the combined content
        float startX = (bounds.getWidth() - totalWidth) * 0.5f;

        // Get color based on state
        juce::Colour colour = isEnabled() ?
            (getToggleState() ? juce::Colours::white : juce::Colours::grey.withAlpha(0.5f))
            : juce::Colours::grey.withAlpha(0.3f);

        // Draw icon
        if (icon != nullptr)
        {
            icon->setColour(0, colour);
            icon->drawWithin(g,
                juce::Rectangle<float>(startX, (bounds.getHeight() - iconSize) * 0.5f,
                    iconSize, iconSize),
                juce::RectanglePlacement::centred, 1.0f);
        }

        // Draw text
        g.setColour(colour);
        g.drawText(getButtonText(),
            juce::Rectangle<float>(startX + iconSize + spacing, 0,
                textWidth, bounds.getHeight()),
            juce::Justification::centredLeft, true);

        // Optional: Draw hover/pressed state
        if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
        {
            g.setColour(juce::Colours::white.withAlpha(0.1f));
            g.fillAll();
        }
    }

private:
    juce::Drawable* icon;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbButton)
};

class HeaderBar : public juce::Component,
    private juce::AudioProcessorValueTreeState::Listener
{
public:
    HeaderBar(juce::AudioProcessorValueTreeState& apvts);
    ~HeaderBar() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onMenuClicked;
    std::function<void()> onLogoClicked;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    // SVG Drawables
    std::unique_ptr<juce::Drawable> logoDrawable;
    std::unique_ptr<juce::Drawable> menuDrawable;
    std::unique_ptr<juce::Drawable> roomDrawable;
    std::unique_ptr<juce::Drawable> plateDrawable;
    std::unique_ptr<juce::Drawable> hallDrawable;
    std::unique_ptr<juce::Drawable> chamberDrawable;
    std::unique_ptr<juce::Drawable> springDrawable;

    // Buttons
    std::unique_ptr<juce::DrawableButton> logoButton;
    std::unique_ptr<ReverbButton> roomButton;
    std::unique_ptr<ReverbButton> plateButton;
    std::unique_ptr<ReverbButton> hallButton;
    std::unique_ptr<ReverbButton> chamberButton;
    std::unique_ptr<ReverbButton> springButton;
    std::unique_ptr<juce::DrawableButton> menuButton;
    std::unique_ptr<juce::Label> versionLabel;

    void initializeComponents();
    void initializeReverbButtons();
    void setupSVGButton(std::unique_ptr<juce::DrawableButton>& button,
        std::unique_ptr<juce::Drawable>& drawable,
        const void* svgData,
        size_t svgDataSize,
        const juce::String& name);

    void handleReverbTypeClick(const juce::String& type);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void updateButtonStates(const juce::String& type);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderBar)
};
