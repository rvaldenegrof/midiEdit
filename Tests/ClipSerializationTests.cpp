#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/Clip.h"
#include "Model/ClipState.h"

using namespace harmonyArchitect::core;
using namespace harmonyArchitect::model;

namespace
{

class ClipSerializationTests : public juce::UnitTest
{
public:
    ClipSerializationTests() : juce::UnitTest("ClipState serializacion", "Model") {}

    void runTest() override
    {
        beginTest("Ida y vuelta Clip -> ValueTree -> Clip conserva el contenido");
        {
            Clip original;
            original.setTimeSignature(TimeSignature(7, 8, {2, 2, 3}));
            original.setKeyRoot(2); // D
            original.setMode(5);    // Eolico
            original.setLengthInBars(4);

            Note kick;
            kick.pitch = 36;
            kick.startTicks = 0;
            kick.durationTicks = 120;
            kick.velocity = 110;
            kick.locked = true;
            original.addNote(kick);

            Note snare;
            snare.pitch = 38;
            snare.startTicks = 480;
            snare.durationTicks = 120;
            snare.velocity = 95;
            snare.locked = false;
            original.addNote(snare);

            juce::UndoManager undoManager;
            ClipState state(undoManager);
            state.loadFrom(original);

            const Clip roundTripped = state.toClip();
            expect(roundTripped == original);
        }

        beginTest("Clip vacio por defecto redondea bien");
        {
            const Clip empty;
            juce::UndoManager undoManager;
            ClipState state(undoManager);
            state.loadFrom(empty);
            expect(state.toClip() == empty);
        }

        beginTest("addNote / setNoteLocked / removeNoteAt pasan por el UndoManager");
        {
            juce::UndoManager undoManager;
            ClipState state(undoManager);
            state.loadFrom(Clip{});

            Note note;
            note.pitch = 60;
            note.startTicks = 0;
            note.durationTicks = 240;
            state.addNote(note);
            expectEquals(state.tree().getNumChildren(), 1);

            undoManager.beginNewTransaction();
            state.setNoteLocked(0, true);
            expect(state.toClip().notes()[0].locked);

            undoManager.undo();
            expect(!state.toClip().notes()[0].locked);

            undoManager.beginNewTransaction();
            state.removeNoteAt(0);
            expectEquals(state.tree().getNumChildren(), 0);

            undoManager.undo();
            expectEquals(state.tree().getNumChildren(), 1);
        }
    }
};

static ClipSerializationTests clipSerializationTests;

} // namespace
