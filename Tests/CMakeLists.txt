# Pruebas de Core/Model sin levantar ningún host (ver CLAUDE.md). Compila
# un ejecutable de consola aparte del plugin, enlazado solo contra los
# módulos JUCE que realmente usa el modelo (core + data_structures).
add_executable(HarmonyArchitectTests
    Main.cpp
    TimeSignatureTests.cpp
    ClipSerializationTests.cpp
    ClipPlayerTests.cpp
    ${CMAKE_SOURCE_DIR}/Source/Core/Clip.cpp
    ${CMAKE_SOURCE_DIR}/Source/Core/ClipPlayer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Model/ClipState.cpp
)

target_include_directories(HarmonyArchitectTests PRIVATE ${CMAKE_SOURCE_DIR}/Source)

target_link_libraries(HarmonyArchitectTests PRIVATE
    juce::juce_core
    juce::juce_data_structures
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

target_compile_definitions(HarmonyArchitectTests PRIVATE
    JUCE_STANDALONE_APPLICATION=1
)
