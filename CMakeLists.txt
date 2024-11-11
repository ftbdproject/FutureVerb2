cmake_minimum_required(VERSION 3.15)

project(FutureVerb2 VERSION 2024.11.11)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and include JUCE
add_subdirectory(JUCE) # Adjust this path to where JUCE is located in your project

# Initialize JUCE
juce_add_plugin(FutureVerb2
    VERSION "2024.11.11"                            # Set this to your plugin version
    COMPANY_NAME "Khaoniewji Development"           # Set this to your company name
    PLUGIN_MANUFACTURER_CODE "Khao"                 # A unique four-character manufacturer id
    PLUGIN_CODE "Ftrv"                             # A unique four-character plugin id
    FORMATS VST3                                    # The formats to build. Other options: AU AAX Unity VST
    PRODUCT_NAME "FutureVerb"                      # The name of your plugin
    COMPANY_WEBSITE "futureboard.app"
    BUNDLE_ID "app.futureboard.ftrvb"
    VST3_CATEGORIES Reverb
)

# Add source files
target_sources(FutureVerb2
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Core/ReverbProcessor.cpp
        Source/Components/ContentArea.cpp
        Source/Components/HeaderBar.cpp
        Source/Components/VuMeter.cpp
)

# Specify include directories
target_include_directories(FutureVerb2
    PRIVATE
        Source
        Source/Core
        Source/Components
)

# Add binary resources
juce_add_binary_data(FutureVerb2_Resources
    SOURCES
        Source/Assets/interfacex1.png
        Source/Assets/interfacex2.png
        Source/Assets/midbutton.svg
        Source/Assets/sidebutton.svg
        Source/Assets/knob2.png
        Source/Assets/background.png
        Source/Assets/background.svg
        Source/Assets/logonewx.svg
        Source/Assets/logonew.svg
        Source/Assets/Icons/chamber.svg
        Source/Assets/Icons/hall.svg
        Source/Assets/Icons/plate.svg
        Source/Assets/Icons/room.svg
        Source/Assets/Icons/spring.svg
        Source/Assets/Fonts/IBMPlexSans-Bold.ttf
        Source/Assets/Fonts/IBMPlexSans-BoldItalic.ttf
        Source/Assets/Fonts/IBMPlexSans-ExtraLight.ttf
        Source/Assets/Fonts/IBMPlexSans-ExtraLightItalic.ttf
        Source/Assets/Fonts/IBMPlexSans-Italic.ttf
        Source/Assets/Fonts/IBMPlexSans-Light.ttf
        Source/Assets/Fonts/IBMPlexSans-LightItalic.ttf
        Source/Assets/Fonts/IBMPlexSans-Medium.ttf
        Source/Assets/Fonts/IBMPlexSans-MediumItalic.ttf
        Source/Assets/Fonts/IBMPlexSans-Regular.ttf
        Source/Assets/Fonts/IBMPlexSans-SemiBold.ttf
        Source/Assets/Fonts/IBMPlexSans-SemiBoldItalic.ttf
        Source/Assets/Fonts/IBMPlexSans-Thin.ttf
        Source/Assets/Fonts/IBMPlexSans-ThinItalic.ttf
        Source/Assets/knob.png
        Source/Assets/logo.svg
        Source/Assets/menu.svg
)

# Link to JUCE modules
target_link_libraries(FutureVerb2
    PRIVATE
        FutureVerb2_Resources
        juce::juce_animation
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_box2d
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
