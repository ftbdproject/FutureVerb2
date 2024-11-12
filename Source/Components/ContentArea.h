// ContentArea.h
#pragma once
#include "../JuceHeader.h"
#include "VuMeter.h"
#include "../CustomLookAndFeel.h"

class ContentArea : public juce::Component
{
public:
    ContentArea(juce::AudioProcessorValueTreeState& apvts);
    ~ContentArea() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void setVuMeterLevels(float leftLevel, float rightLevel);

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    CustomLookAndFeel customLookAndFeel;

    // With this single button
    std::unique_ptr<juce::DrawableButton> midSideButton;

    // Knobs
    juce::Slider roomSizeKnob;
    juce::Slider decayKnob;
    juce::Slider modulationKnob;
    juce::Slider diffusionKnob;
    juce::Slider speedKnob;
    juce::Slider bassKnob;
    juce::Slider crossKnob;
    juce::Slider dampingKnob;
    juce::Slider lowpassKnob;
    juce::Slider highpassKnob;
    juce::Slider delayTimeKnob;
    juce::Slider feedbackKnob;
    juce::Slider pitchShiftKnob;
    juce::Slider mixKnob;
    juce::Slider erLevelKnob;
    juce::Slider stereoSepKnob;
    juce::Slider outputKnob;

    // VU Meters
    VuMeter leftMeter;
    VuMeter rightMeter;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> midSideAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highpassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowpassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modulationAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diffusionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> erLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoSepAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    void setupKnob(juce::Slider& knob,
        const juce::String& paramID,
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void setupMidSideButtons();
    void setupVuMeter(VuMeter& meter);
    void removeAllLookAndFeels();
    void layoutControls(int totalWidth, int totalHeight);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContentArea)
};
