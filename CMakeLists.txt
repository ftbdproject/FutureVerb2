cmake_minimum_required(VERSION 3.15)

project(FutureVerb2 VERSION 2024.11.11)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and include JUCE
add_subdirectory(JUCE)

# Initialize JUCE plugin with binary resources
juce_add_plugin(FutureVerb2
    VERSION "2024.11.11"
    COMPANY_NAME "Khaoniewji Development"
    PLUGIN_MANUFACTURER_CODE "Khao"
    PLUGIN_CODE "Ftrv"
    FORMATS VST3
    PRODUCT_NAME "FutureVerb"
    COMPANY_WEBSITE "futureboard.app"
    BUNDLE_ID "app.futureboard.ftrvb"
    VST3_CATEGORIES Reverb
    BINARY_DATA
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
        Source/Assets/irwave.svg
        Source/Assets/customir.svg
)

# Generate JUCE header
juce_generate_juce_header(FutureVerb2)

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

# Link libraries
target_link_libraries(FutureVerb2
    PRIVATE
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

target_compile_definitions(FutureVerb2
    PRIVATE
        $<$<PLATFORM_ID:Windows>:__WINDOWS_DS__>
        $<$<PLATFORM_ID:Windows>:__LITTLE_ENDIAN__>
        _USE_MATH_DEFINES
)