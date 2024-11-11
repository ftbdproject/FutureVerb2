// ContentArea.cpp
#include "ContentArea.h"

ContentArea::ContentArea(juce::AudioProcessorValueTreeState& apvts)
    : valueTreeState(apvts)
{
    setOpaque(false);

    // Setup Mid/Side buttons
    setupMidSideButtons();

    // Setup all knobs with correct parameter IDs
    setupKnob(roomSizeKnob, "size", roomSizeAttachment);
    setupKnob(decayKnob, "decay", decayAttachment);
    setupKnob(modulationKnob, "modulation", modulationAttachment);
    setupKnob(diffusionKnob, "diffusion", diffusionAttachment);
    setupKnob(speedKnob, "speed", speedAttachment);
    setupKnob(bassKnob, "bass", bassAttachment);
    setupKnob(crossKnob, "cross", crossAttachment);
    setupKnob(dampingKnob, "damping", dampingAttachment);
    setupKnob(lowpassKnob, "lowpass", lowpassAttachment);
    setupKnob(highpassKnob, "highpass", highpassAttachment);
    setupKnob(delayTimeKnob, "predelay", delayTimeAttachment);
    setupKnob(feedbackKnob, "decay", feedbackAttachment);
    setupKnob(mixKnob, "mix", mixAttachment);
    setupKnob(erLevelKnob, "erlevel", erLevelAttachment);
    setupKnob(stereoSepKnob, "stereosep", stereoSepAttachment);
    setupKnob(outputKnob, "output", outputAttachment);
    // Remove this line since it's handled in setupKnob
    // outputKnob.setDoubleClickReturnValue(true, 0.0);

    // Setup VU meters
    setupVuMeter(leftMeter);
    setupVuMeter(rightMeter);
}

void ContentArea::setupMidSideButtons()
{
    auto midSideSvg = juce::Drawable::createFromImageData(BinaryData::midbutton_svg,
        BinaryData::midbutton_svgSize);

    midSideButton.reset(new juce::DrawableButton("MidSide", juce::DrawableButton::ButtonStyle::ImageFitted));
    midSideButton->setImages(midSideSvg.get());
    addAndMakeVisible(midSideButton.get());

    // Add the attachment for the Mid/Side button
    midSideAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        valueTreeState, "midside", *midSideButton);
}

void ContentArea::setupKnob(juce::Slider& knob,
    const juce::String& paramID,
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    knob.setLookAndFeel(&customLookAndFeel);

    // Special handling for output knob
    if (paramID == "output")
    {
        knob.setRange(-48.0, 12.0, 0.1);
        knob.setValue(0.0);
        knob.setDoubleClickReturnValue(true, 0.0);
        // Optional: Add skew factor for more precise control around 0 dB
        knob.setSkewFactorFromMidPoint(0.0);
    }

    addAndMakeVisible(knob);

    if (auto* param = valueTreeState.getParameter(paramID))
    {
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            valueTreeState, paramID, knob);
    }
    else
    {
        jassertfalse; // Parameter ID not found!
    }
}

void ContentArea::setupVuMeter(VuMeter& meter)
{
    meter.setDecibelsRange(-60.0f, 6.0f);
    meter.setNumSegments(0);
    addAndMakeVisible(meter);
}

void ContentArea::resized()
{
    layoutControls(getWidth(), getHeight());
}

void ContentArea::layoutControls(int totalWidth, int totalHeight)
{
    // Knob and layout settings
    const int knobSize = 60;
    const int knobSpacing = 10;
    const int sectionSpacing = 43;
    const int verticalSpacing = 30;

    // Section widths
    const int leftSectionWidth = knobSize * 4 + knobSpacing * 3;
    const int filterSectionWidth = knobSize * 2 + knobSpacing;
    const int pitchSectionWidth = knobSize + sectionSpacing + knobSpacing;
    const int delaySectionWidth = knobSize * 2 + knobSpacing;
    const int rightSectionWidth = knobSize * 2 + knobSpacing;

    // Calculate starting X and Y positions to center everything
    const int contentWidth = leftSectionWidth + filterSectionWidth + pitchSectionWidth + delaySectionWidth + rightSectionWidth;
    const int startX = (totalWidth - contentWidth) / 8;
    const int startY = (totalHeight - (knobSize * 2 + verticalSpacing)) / 1.3;

    int currentX = startX;

    // Left section (Room Size, Decay, etc.)
    roomSizeKnob.setBounds(currentX, startY, knobSize, knobSize);
    decayKnob.setBounds(currentX + knobSize + knobSpacing, startY, knobSize, knobSize);
    modulationKnob.setBounds(currentX + (knobSize + knobSpacing) * 2, startY, knobSize, knobSize);
    diffusionKnob.setBounds(currentX + (knobSize + knobSpacing) * 3, startY, knobSize, knobSize);

    speedKnob.setBounds(currentX, startY + knobSize + verticalSpacing, knobSize, knobSize);
    bassKnob.setBounds(currentX + knobSize + knobSpacing, startY + knobSize + verticalSpacing, knobSize, knobSize);
    crossKnob.setBounds(currentX + (knobSize + knobSpacing) * 2, startY + knobSize + verticalSpacing, knobSize, knobSize);
    dampingKnob.setBounds(currentX + (knobSize + knobSpacing) * 3, startY + knobSize + verticalSpacing, knobSize, knobSize);

    // Move to filter section
    currentX += leftSectionWidth + sectionSpacing;

    lowpassKnob.setBounds(currentX, startY, knobSize, knobSize);
    highpassKnob.setBounds(currentX, startY + knobSize + verticalSpacing, knobSize, knobSize);

    // Move to delay section
    currentX += pitchSectionWidth + sectionSpacing;

    delayTimeKnob.setBounds(currentX, startY, knobSize, knobSize);
    feedbackKnob.setBounds(currentX, startY + knobSize + verticalSpacing, knobSize, knobSize);

    // Mid/Side button
    // Move to Mid/Side and Pitch section
    currentX += filterSectionWidth + sectionSpacing;
    const int buttonSize = knobSize / 1;
    const int buttonY = startY + (knobSize - buttonSize) / 3;
    midSideButton->setBounds(currentX, buttonY, buttonSize, buttonSize);

    // Move to output section
    currentX += delaySectionWidth + sectionSpacing;

    mixKnob.setBounds(currentX, startY, knobSize, knobSize);
    erLevelKnob.setBounds(currentX + knobSize + knobSpacing, startY, knobSize, knobSize);
    outputKnob.setBounds(currentX, startY + knobSize + verticalSpacing, knobSize, knobSize);
    stereoSepKnob.setBounds(currentX + knobSize + knobSpacing, startY + knobSize + verticalSpacing, knobSize, knobSize);
}


ContentArea::~ContentArea()
{
    removeAllLookAndFeels();
}

void ContentArea::removeAllLookAndFeels()
{
    roomSizeKnob.setLookAndFeel(nullptr);
    decayKnob.setLookAndFeel(nullptr);
    modulationKnob.setLookAndFeel(nullptr);
    diffusionKnob.setLookAndFeel(nullptr);
    speedKnob.setLookAndFeel(nullptr);
    bassKnob.setLookAndFeel(nullptr);
    crossKnob.setLookAndFeel(nullptr);
    dampingKnob.setLookAndFeel(nullptr);
    lowpassKnob.setLookAndFeel(nullptr);
    highpassKnob.setLookAndFeel(nullptr);
    delayTimeKnob.setLookAndFeel(nullptr);
    feedbackKnob.setLookAndFeel(nullptr);
    pitchShiftKnob.setLookAndFeel(nullptr);
    mixKnob.setLookAndFeel(nullptr);
    erLevelKnob.setLookAndFeel(nullptr);
    stereoSepKnob.setLookAndFeel(nullptr);
    outputKnob.setLookAndFeel(nullptr);
}

void ContentArea::paint(juce::Graphics& g)
{
}

void ContentArea::setVuMeterLevels(float leftLevel, float rightLevel)
{
    leftMeter.setValue(leftLevel);
    rightMeter.setValue(rightLevel);
}