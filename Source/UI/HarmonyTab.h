#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace harmonyArchitect
{

// Contenido de la pestaña "Armonía y Melodía". Vacío por ahora (Fase 0 del
// roadmap); se completa recién en el Bloque C, después de terminar Ritmos.
class HarmonyTab : public juce::Component
{
public:
    HarmonyTab();

    void paint(juce::Graphics& g) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonyTab)
};

} // namespace harmonyArchitect
