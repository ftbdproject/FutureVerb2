/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 9 Nov 2024 10:18:57pm
    Author:  KinoBeddiez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Load IBM Plex Sans font
        auto fontData = BinaryData::IBMPlexSansRegular_ttf;
        auto fontDataSize = BinaryData::IBMPlexSansRegular_ttfSize;

        if (fontData != nullptr)
        {
            customTypeface = juce::Typeface::createSystemTypefaceFor(fontData, fontDataSize);
            // Create font with the loaded typeface
            customFont = juce::Font(customTypeface).withHeight(14.0f);
        }

        // Set dark theme colors
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF1E1E1E));
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2D2D30));
        setColour(juce::TextButton::textColourOffId, juce::Colour(0xFFCCCCCC));
        setColour(juce::TextButton::textColourOnId, juce::Colour(0xFFFFFFFF));

        setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF2D2D30));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF007ACC));
        setColour(juce::Slider::trackColourId, juce::Colour(0xFF007ACC));
        setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFE0E0E0));
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF2D2D30));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF3E3E42));

        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF2D2D30));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xFFE0E0E0));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF3E3E42));

        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF2D2D30));
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xFFE0E0E0));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF007ACC));

        setColour(juce::Label::textColourId, juce::Colour(0xFFE0E0E0));
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(13.0f).withStyle(juce::Font::bold);
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return customFont;
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return customFont;
    }

    juce::Font getPopupMenuFont() override
    {
        return customFont;
    }

    void drawButtonBackground(juce::Graphics& g,
                            juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override
    {
        // Don't draw any background for TextButtons
        if (dynamic_cast<juce::TextButton*>(&button) != nullptr)
            return;

        // For other buttons, use default behavior
        LookAndFeel_V4::drawButtonBackground(g, button, backgroundColour,
                                           shouldDrawButtonAsHighlighted,
                                           shouldDrawButtonAsDown);
    }

    // Override button content position to remove padding
    void positionButtonText(juce::Button& button, juce::Label& label)
    {
        label.setBounds(button.getLocalBounds());
        label.setJustificationType(juce::Justification::centred);
    }

    //// Override to remove button border
    //int getTextButtonBorderSize(juce::TextButton&) override
    //{
    //    return 0;
    //}

    //// Override to remove button edge indentation
    //int getTextButtonEdgeIndent() override
    //{
    //    return 0;
    //}
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider& slider) override
    {
        // Get the image directly without pointer
        juce::Image knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);

        if (!knobImage.isNull())
        {
            const double rotation = (sliderPos - 0.5) * juce::MathConstants<float>::pi;
            const int frames = 128;
            const int frameWidth = knobImage.getWidth() / frames;
            const int frameHeight = knobImage.getHeight();

            // Calculate which frame to show based on rotation
            const int frameIndex = (int)((sliderPos * (frames - 1)) + 0.5);
            const int clampedFrameIndex = juce::jlimit(0, frames - 1, frameIndex);

            // Calculate positioning to center the knob
            const float scale = juce::jmin(width / (float)frameWidth, height / (float)frameHeight);
            const float scaledWidth = frameWidth * scale;
            const float scaledHeight = frameHeight * scale;
            const float centerX = x + (width - scaledWidth) * 0.5f;
            const float centerY = y + (height - scaledHeight) * 0.5f;

            // Draw the appropriate frame
            g.drawImage(knobImage,
                (int)centerX, (int)centerY, (int)scaledWidth, (int)scaledHeight,
                frameWidth * clampedFrameIndex, 0, frameWidth, frameHeight);
        }
    }
    void updateKnobImage(juce::Slider& slider)
    {
        // Set mouse cursor behavior
        slider.setMouseDragSensitivity(128);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    }

private:
    juce::Font customFont;
    juce::Typeface::Ptr customTypeface;
};
