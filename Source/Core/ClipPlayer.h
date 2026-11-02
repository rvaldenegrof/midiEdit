#pragma once

#include <cstdint>
#include <vector>

#include "Clip.h"

namespace harmonyArchitect::core
{

// Un evento de nota (on/off) que cae dentro del rango de ticks consultado.
// tickInRange es relativo al inicio de ese rango (0..rangeLength), listo
// para convertir a un offset de muestra dentro de un bloque de audio.
struct PlaybackEvent
{
    std::int64_t tickInRange = 0;
    int pitch = 60;
    int velocity = 100;
    bool isNoteOn = true;
};

// Recorre un Clip, que se repite en loop cada clip.lengthInTicks(), y
// llena outEvents con los eventos de nota que caen en el rango de ticks
// absolutos [startTick, startTick + rangeLength). Pensado para llamarse
// una vez por bloque de audio con el rango que cubre ese bloque.
//
// No reserva memoria: limpia y reutiliza outEvents (push_back sin
// reallocar siempre que el llamador le haya reservado capacidad de
// sobra de antemano), para poder llamarse desde el hilo de audio.
// Asume que ninguna nota dura más que un ciclo de loop completo.
void collectPlaybackEvents(const Clip& clip,
                            std::int64_t startTick,
                            std::int64_t rangeLength,
                            std::vector<PlaybackEvent>& outEvents);

} // namespace harmonyArchitect::core
