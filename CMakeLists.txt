cmake_minimum_required(VERSION 3.22)

project(HarmonyArchitect VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/third_party")

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/JUCE"
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(HarmonyArchitect
    COMPANY_NAME "HarmonyArchitect"
    PLUGIN_MANUFACTURER_CODE Hmac
    PLUGIN_CODE Hmar
    FORMATS VST3 Standalone
    PRODUCT_NAME "Harmony Architect"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
)

target_sources(HarmonyArchitect PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/RhythmTab.cpp
    Source/UI/HarmonyTab.cpp
    Source/Core/Clip.cpp
    Source/Model/ClipState.cpp
)

target_include_directories(HarmonyArchitect PRIVATE Source)

target_compile_definitions(HarmonyArchitect PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(HarmonyArchitect
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

option(HARMONY_ARCHITECT_BUILD_TESTS "Compilar los tests de Core/Model (Tests/)" ON)
if (HARMONY_ARCHITECT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
    add_test(NAME HarmonyArchitectTests COMMAND HarmonyArchitectTests)
endif()
