/*
  ==============================================================================

    Preset.h
    Created: 11 Nov 2024 3:48:06pm
    Author:  KinoBeddiez

  ==============================================================================
*/

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

struct Preset
{
    juce::String name;
    float size;
    float diffusion;
    float decay;
    float damping;
    float width;
    float preDelay;
    float mix;
    float modulation;
    float speed;
    float bass;
    float cross;
    float erLevel;
    float stereoSep;
    float highpass;
    float lowpass;
    float output;
    bool midSide;
    int type;
};

class PresetManager
{
public:
    static const std::vector<Preset> getFactoryPresets()
    {
        return {
            // Small Room
            {"Small Room", 0.2f, 0.4f, 0.8f, 0.5f, 0.8f, 0.0f, 0.3f, 0.1f, 0.3f, 0.5f, 0.2f, 0.4f, 0.5f, 20.0f, 18000.0f, 0.0f, false, 0},

            // Large Hall
            {"Large Hall", 0.8f, 0.7f, 4.0f, 0.3f, 1.0f, 20.0f, 0.4f, 0.2f, 0.4f, 0.6f, 0.3f, 0.6f, 0.7f, 20.0f, 16000.0f, 0.0f, false, 2},

            // Plate
            {"Plate", 0.5f, 0.8f, 2.5f, 0.4f, 0.9f, 10.0f, 0.35f, 0.15f, 0.5f, 0.4f, 0.25f, 0.5f, 0.6f, 100.0f, 18000.0f, 0.0f, false, 1},

            // Chamber
            {"Chamber", 0.6f, 0.6f, 1.8f, 0.45f, 0.85f, 15.0f, 0.3f, 0.1f, 0.4f, 0.5f, 0.3f, 0.45f, 0.55f, 50.0f, 16000.0f, 0.0f, false, 3},

            // Spring
            {"Spring", 0.4f, 0.75f, 2.2f, 0.6f, 0.7f, 5.0f, 0.4f, 0.3f, 0.6f, 0.7f, 0.4f, 0.3f, 0.4f, 150.0f, 12000.0f, 0.0f, false, 4}
        };
    }
};
