#include "ClipPlayer.h"

#include <algorithm>

namespace harmonyArchitect::core
{

namespace
{

std::int64_t floorDiv(std::int64_t a, std::int64_t b)
{
    const std::int64_t q = a / b;
    const std::int64_t r = a % b;
    return (r != 0 && ((r < 0) != (b < 0))) ? q - 1 : q;
}

} // namespace

void collectPlaybackEvents(const Clip& clip,
                            std::int64_t startTick,
                            std::int64_t rangeLength,
                            std::vector<PlaybackEvent>& outEvents)
{
    outEvents.clear();

    const std::int64_t loopLength = clip.lengthInTicks();
    if (loopLength <= 0 || rangeLength <= 0 || clip.notes().empty())
        return;

    const std::int64_t rangeEnd = startTick + rangeLength; // exclusivo

    // Un ciclo de margen a cada lado alcanza siempre que ninguna nota dure
    // más que un loop completo (ver comentario en el header).
    const std::int64_t firstCycle = floorDiv(startTick, loopLength) - 1;
    const std::int64_t lastCycle = floorDiv(rangeEnd, loopLength) + 1;

    for (std::int64_t cycle = firstCycle; cycle <= lastCycle; ++cycle)
    {
        const std::int64_t cycleBase = cycle * loopLength;

        for (const auto& note : clip.notes())
        {
            const std::int64_t noteOnAbs = cycleBase + note.startTicks;
            if (noteOnAbs >= startTick && noteOnAbs < rangeEnd)
                outEvents.push_back({noteOnAbs - startTick, note.pitch, note.velocity, true});

            const std::int64_t noteOffAbs = noteOnAbs + note.durationTicks;
            if (noteOffAbs >= startTick && noteOffAbs < rangeEnd)
                outEvents.push_back({noteOffAbs - startTick, note.pitch, note.velocity, false});
        }
    }

    // std::sort (no stable_sort) para no arriesgar una asignación interna
    // en el hilo de audio. El desempate hace que un note-off siempre
    // preceda a un note-on en el mismo tick -- soltar antes de volver a
    // pulsar la misma nota es siempre seguro en MIDI.
    std::sort(outEvents.begin(), outEvents.end(), [](const PlaybackEvent& a, const PlaybackEvent& b) {
        if (a.tickInRange != b.tickInRange)
            return a.tickInRange < b.tickInRange;
        return !a.isNoteOn && b.isNoteOn;
    });
}

} // namespace harmonyArchitect::core
