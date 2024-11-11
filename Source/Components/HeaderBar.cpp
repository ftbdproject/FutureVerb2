// HeaderBar.cpp
#include "HeaderBar.h"

HeaderBar::HeaderBar(juce::AudioProcessorValueTreeState& apvts)
    : valueTreeState(apvts)
{
    setSize(800, 40);
    initializeComponents();

    // Start listening to parameter changes
    valueTreeState.addParameterListener("type", this);
}

HeaderBar::~HeaderBar()
{
    valueTreeState.removeParameterListener("type", this);

    // Clean up buttons
    logoButton = nullptr;
    roomButton = nullptr;
    plateButton = nullptr;
    hallButton = nullptr;
    chamberButton = nullptr;
    springButton = nullptr;
    menuButton = nullptr;
}

void HeaderBar::setupSVGButton(std::unique_ptr<juce::DrawableButton>& button,
    std::unique_ptr<juce::Drawable>& drawable,
    const void* svgData,
    size_t svgDataSize,
    const juce::String& name)
{
    if (svgData == nullptr)
        return;

    drawable = juce::Drawable::createFromImageData(svgData, svgDataSize);
    if (drawable == nullptr)
        return;

    button = std::make_unique<juce::DrawableButton>(name, juce::DrawableButton::ImageFitted);
    if (button != nullptr)
    {
        button->setImages(drawable.get());
        button->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        button->setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button.get());

        if (name == "Logo")
        {
            button->onClick = [this]() {
                if (onLogoClicked) onLogoClicked();
                };
        }
        else if (name == "Menu")
        {
            button->onClick = [this]() {
                if (onMenuClicked) onMenuClicked();
                };
        }
    }
}

void HeaderBar::initializeComponents()
{
    // Setup Logo SVG
    setupSVGButton(logoButton, logoDrawable,
        BinaryData::logonewx_svg,
        BinaryData::logonewx_svgSize,
        "Logo");

    // Initialize reverb buttons
    initializeReverbButtons();

    // Setup Menu SVG
    setupSVGButton(menuButton, menuDrawable,
        BinaryData::menu_svg,
        BinaryData::menu_svgSize,
        "Menu");

    // Version label
    versionLabel = std::make_unique<juce::Label>("version", "v2024.12.15");
    if (versionLabel != nullptr)
    {
        versionLabel->setJustificationType(juce::Justification::centredRight);
        versionLabel->setColour(juce::Label::textColourId, juce::Colours::grey);
        addAndMakeVisible(versionLabel.get());
    }
}

void HeaderBar::initializeReverbButtons()
{
    auto createButton = [this](std::unique_ptr<ReverbButton>& button,
        std::unique_ptr<juce::Drawable>& drawable,
        const void* svgData,
        size_t svgDataSize,
        const juce::String& name)
        {
            drawable = juce::Drawable::createFromImageData(svgData, svgDataSize);

            button = std::make_unique<ReverbButton>(name, drawable.get());
            if (button != nullptr)
            {
                button->setRadioGroupId(1);
                button->onClick = [this, name]() { handleReverbTypeClick(name); };
                button->setMouseCursor(juce::MouseCursor::PointingHandCursor);
                addAndMakeVisible(button.get());
            }
        };

    // Create buttons with their respective SVGs
    createButton(roomButton, roomDrawable, BinaryData::room_svg, BinaryData::room_svgSize, "ROOM");
    createButton(plateButton, plateDrawable, BinaryData::plate_svg, BinaryData::plate_svgSize, "PLATE");
    createButton(hallButton, hallDrawable, BinaryData::hall_svg, BinaryData::hall_svgSize, "HALL");
    createButton(chamberButton, chamberDrawable, BinaryData::chamber_svg, BinaryData::chamber_svgSize, "CHAMBER");
    createButton(springButton, springDrawable, BinaryData::spring_svg, BinaryData::spring_svgSize, "SPRING");

    // Set initial state based on parameter value
    if (auto* param = valueTreeState.getParameter("type"))
    {
        const juce::StringArray types = { "ROOM", "PLATE", "HALL", "CHAMBER", "SPRING" };
        int index = static_cast<int>(param->getValue() * (types.size() - 1));
        updateButtonStates(types[index]);
    }
}

void HeaderBar::handleReverbTypeClick(const juce::String& type)
{
    const juce::StringArray types = { "ROOM", "PLATE", "HALL", "CHAMBER", "SPRING" };
    int typeIndex = types.indexOf(type);

    if (auto* param = valueTreeState.getParameter("type"))
    {
        param->setValueNotifyingHost(typeIndex / float(types.size() - 1));
    }
}

void HeaderBar::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "type")
    {
        const juce::StringArray types = { "ROOM", "PLATE", "HALL", "CHAMBER", "SPRING" };
        int index = static_cast<int>(newValue * (types.size() - 1));
        if (index >= 0 && index < types.size())
        {
            juce::MessageManager::callAsync([this, type = types[index]]() {
                updateButtonStates(type);
                });
        }
    }
}

void HeaderBar::updateButtonStates(const juce::String& type)
{
    if (roomButton) roomButton->setToggleState(type == "ROOM", juce::dontSendNotification);
    if (plateButton) plateButton->setToggleState(type == "PLATE", juce::dontSendNotification);
    if (hallButton) hallButton->setToggleState(type == "HALL", juce::dontSendNotification);
    if (chamberButton) chamberButton->setToggleState(type == "CHAMBER", juce::dontSendNotification);
    if (springButton) springButton->setToggleState(type == "SPRING", juce::dontSendNotification);
}

void HeaderBar::resized()
{
    auto bounds = getLocalBounds();
    if (bounds.isEmpty())
        return;

    const int logoWidth = 108;
    const int menuWidth = 22;
    const int versionWidth = 120;
    const int horizontalSpacing = 8;
    const int buttonSpacing = 4;
    const int verticalMargin = 0;
    const int reverbGroupWidth = 450;

    // Logo section
    if (logoButton != nullptr)
    {
        logoButton->setBounds(horizontalSpacing, 0, logoWidth, bounds.getHeight());
    }

    // Right-side controls
    if (menuButton != nullptr)
    {
        menuButton->setBounds(bounds.getWidth() - menuWidth - horizontalSpacing,
            0, menuWidth, bounds.getHeight());
    }

    if (versionLabel != nullptr)
    {
        versionLabel->setBounds(bounds.getWidth() - menuWidth - versionWidth - horizontalSpacing * 2,
            0, versionWidth, bounds.getHeight());
    }

    // Calculate reverb buttons area
    const int reverbAreaX = (bounds.getWidth() - reverbGroupWidth) / 2;
    const int totalButtonSpacing = buttonSpacing * 4;
    const int buttonWidth = (reverbGroupWidth - totalButtonSpacing) / 5;
    const int buttonHeight = bounds.getHeight() - (verticalMargin * 2);
    const int buttonY = verticalMargin;

    // Position reverb buttons
    if (roomButton != nullptr)
        roomButton->setBounds(reverbAreaX, buttonY, buttonWidth, buttonHeight);

    if (plateButton != nullptr)
        plateButton->setBounds(reverbAreaX + buttonWidth + buttonSpacing,
            buttonY, buttonWidth, buttonHeight);

    if (hallButton != nullptr)
        hallButton->setBounds(reverbAreaX + (buttonWidth + buttonSpacing) * 2,
            buttonY, buttonWidth, buttonHeight);

    if (chamberButton != nullptr)
        chamberButton->setBounds(reverbAreaX + (buttonWidth + buttonSpacing) * 3,
            buttonY, buttonWidth, buttonHeight);

    if (springButton != nullptr)
        springButton->setBounds(reverbAreaX + (buttonWidth + buttonSpacing) * 4,
            buttonY, buttonWidth, buttonHeight);
}

void HeaderBar::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    if (bounds.isEmpty())
        return;

    g.setColour(juce::Colour(0xFF1E1E1E));
    g.drawHorizontalLine(bounds.getHeight() - 1, 0.0f, static_cast<float>(getWidth()));
}