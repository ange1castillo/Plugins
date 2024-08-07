cmake_minimum_required(VERSION 3.29.5)

project(Synthesizer VERSION 0.1.0)

set(INCLUDE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/Data")
set(INCLUDE_SYNTHESIZER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/Synthesizer")
set(INCLUDE_UI_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/UI")

set(SOURCE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/Data")
set(SOURCE_SYNTHESIZER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/Synthesizer")
set(SOURCE_UI_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/UI")

# Adds a plugin target
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME Gello # change this
    IS_SYNTH TRUE # may change this
    NEEDS_MIDI_INPUT TRUE # may change this
    NEEDS_MIDI_OUTPUT FALSE # may change this
    PLUGIN_MANUFACTURER_CODE Ovte # change this
    PLUGIN_CODE Over # change this
    FORMATS Standalone AU # may change this
    PRODUCT_NAME "Synthesizer" # change this
)

juce_generate_juce_header(${PROJECT_NAME})

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME}
    PRIVATE
        ${INCLUDE_DATA_DIR}/AdsrData.h
        ${SOURCE_DATA_DIR}/AdsrData.cpp

        ${SOURCE_SYNTHESIZER_DIR}/PluginEditor.cpp
        ${SOURCE_SYNTHESIZER_DIR}/PluginProcessor.cpp
        ${SOURCE_SYNTHESIZER_DIR}/SynthVoice.cpp
        ${INCLUDE_SYNTHESIZER_DIR}/PluginEditor.h
        ${INCLUDE_SYNTHESIZER_DIR}/PluginProcessor.h
        ${INCLUDE_SYNTHESIZER_DIR}/SynthSound.h
        ${INCLUDE_SYNTHESIZER_DIR}/SynthVoice.h

        ${INCLUDE_UI_DIRECTORY}/AdsrComponent.h
        ${SOURCE_UI_DIRECTORY}/AdsrComponent.cpp
)

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ../libs/juce/examples/DemoRunner/JuceLibraryCode
)

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall /WX)
    # Silences some deprecation warnings that (hopefully) will be fixed in a future release of JUCE.
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
