#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace harmonyArchitect
{

// Contenido de la pestaña "Ritmos". Vacío por ahora (Fase 0 del roadmap);
// el editor de rejilla y el generador se conectan en fases posteriores.
class RhythmTab : public juce::Component
{
public:
    RhythmTab();

    void paint(juce::Graphics& g) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RhythmTab)
};

} // namespace harmonyArchitect
