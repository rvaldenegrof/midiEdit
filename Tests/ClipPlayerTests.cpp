#include <juce_core/juce_core.h>

#include "Core/ClipPlayer.h"

using namespace harmonyArchitect::core;

namespace
{

Clip makeTestClip()
{
    Clip clip;
    clip.setTimeSignature(TimeSignature(4, 4));
    clip.setLengthInBars(1); // 1 compas de 4/4 = 3840 ticks

    Note kick;
    kick.pitch = 36;
    kick.startTicks = 0;
    kick.durationTicks = 100;
    kick.velocity = 110;
    clip.addNote(kick);

    Note snare;
    snare.pitch = 38;
    snare.startTicks = 1920; // mitad del compas
    snare.durationTicks = 100;
    snare.velocity = 95;
    clip.addNote(snare);

    return clip;
}

class ClipPlayerTests : public juce::UnitTest
{
public:
    ClipPlayerTests() : juce::UnitTest("ClipPlayer", "Core") {}

    void runTest() override
    {
        const Clip clip = makeTestClip();
        std::vector<PlaybackEvent> events;

        beginTest("Rango que cubre el note-on y el note-off del kick");
        {
            collectPlaybackEvents(clip, 0, 200, events);
            expectEquals(static_cast<int>(events.size()), 2);
            expect(events[0].isNoteOn);
            expectEquals(events[0].tickInRange, static_cast<std::int64_t>(0));
            expectEquals(events[0].pitch, 36);
            expect(!events[1].isNoteOn);
            expectEquals(events[1].tickInRange, static_cast<std::int64_t>(100));
        }

        beginTest("Rango que no toca ninguna nota devuelve vacio");
        {
            collectPlaybackEvents(clip, 500, 100, events);
            expect(events.empty());
        }

        beginTest("El loop repite el clip: el segundo ciclo dispara el kick de nuevo");
        {
            const std::int64_t loopLen = clip.lengthInTicks();
            collectPlaybackEvents(clip, loopLen, 50, events);
            expectEquals(static_cast<int>(events.size()), 1);
            expect(events[0].isNoteOn);
            expectEquals(events[0].pitch, 36);
        }

        beginTest("Un rango que cruza el limite del loop ve el final de un ciclo y el inicio del siguiente");
        {
            const std::int64_t loopLen = clip.lengthInTicks();
            // [loopLen - 50, loopLen + 50): nada de la snare (termina en
            // 2020), pero si el note-on del kick del siguiente ciclo.
            collectPlaybackEvents(clip, loopLen - 50, 100, events);
            expectEquals(static_cast<int>(events.size()), 1);
            expect(events[0].isNoteOn);
            expectEquals(events[0].tickInRange, static_cast<std::int64_t>(50));
        }

        beginTest("Note-off antes que note-on cuando coinciden en el mismo tick");
        {
            Clip overlapping;
            overlapping.setTimeSignature(TimeSignature(4, 4));
            overlapping.setLengthInBars(1);

            Note a;
            a.pitch = 60;
            a.startTicks = 0;
            a.durationTicks = 100;
            overlapping.addNote(a);

            Note b;
            b.pitch = 60;
            b.startTicks = 100; // arranca justo donde termina "a"
            b.durationTicks = 100;
            overlapping.addNote(b);

            collectPlaybackEvents(overlapping, 90, 30, events);
            expectEquals(static_cast<int>(events.size()), 2);
            expectEquals(events[0].tickInRange, static_cast<std::int64_t>(10));
            expect(!events[0].isNoteOn);
            expectEquals(events[1].tickInRange, static_cast<std::int64_t>(10));
            expect(events[1].isNoteOn);
        }

        beginTest("Clip sin notas o sin longitud no revienta y devuelve vacio");
        {
            Clip empty;
            collectPlaybackEvents(empty, 0, 100, events);
            expect(events.empty());
        }
    }
};

static ClipPlayerTests clipPlayerTests;

} // namespace
