#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "../Core/Clip.h"

namespace harmonyArchitect::model
{

// Puente entre harmonyArchitect::core::Clip (datos planos, sin JUCE) y un
// juce::ValueTree editable con undo. El piano roll (fases posteriores)
// edita siempre a través de este ValueTree -- nunca guarda estado propio
// de notas -- y el hilo de audio nunca lo toca directamente.
class ClipState
{
public:
    explicit ClipState(juce::UndoManager& undoManager);

    // Reemplaza el contenido del ValueTree a partir de un Clip (p. ej. al
    // generar o cargar un preset). Es una carga, no una edición
    // incremental del usuario, así que no pasa por el UndoManager.
    void loadFrom(const core::Clip& clip);

    // Reconstruye un Clip plano a partir del estado actual del
    // ValueTree (p. ej. para pasarlo a un transformador o al motor de
    // salida MIDI).
    core::Clip toClip() const;

    // Ediciones incrementales: pasan por el UndoManager, a diferencia de
    // loadFrom(). El editor de piano roll (fases posteriores) se apoya en
    // estas mismas operaciones en vez de mutar el ValueTree a mano.
    void addNote(const core::Note& note);
    void removeNoteAt(int index);
    void setNoteLocked(int index, bool locked);

    juce::ValueTree& tree() noexcept { return tree_; }
    const juce::ValueTree& tree() const noexcept { return tree_; }

private:
    juce::UndoManager& undoManager_;
    juce::ValueTree tree_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipState)
};

} // namespace harmonyArchitect::model
