#pragma once

#include <cstdint>
#include <vector>

namespace harmonyArchitect::core
{

// Todas las posiciones están en ticks absolutos relativos al inicio del
// Clip (960 PPQ); nunca segundos ni píxeles (ver CLAUDE.md).
struct Note
{
    int pitch = 60;
    std::int64_t startTicks = 0;
    std::int64_t durationTicks = 0;
    int velocity = 100;
    bool locked = false;

    std::int64_t endTicks() const noexcept { return startTicks + durationTicks; }
};

bool operator==(const Note& a, const Note& b);
bool operator!=(const Note& a, const Note& b);

// Representa un compás, posiblemente irregular (7/8, 11/8...), con su
// agrupación interna en pulsos (p. ej. 7/8 como 2+2+3). Sin agrupación
// explícita, se asume un pulso por figura del denominador.
class TimeSignature
{
public:
    static constexpr int ticksPerQuarterNote = 960;

    TimeSignature() = default;
    TimeSignature(int numerator, int denominator);
    TimeSignature(int numerator, int denominator, std::vector<int> grouping);

    int numerator() const noexcept { return numerator_; }
    int denominator() const noexcept { return denominator_; }
    const std::vector<int>& grouping() const noexcept { return grouping_; }

    // Duración del compás completo, en ticks.
    std::int64_t barLengthInTicks() const;

    // Longitud en ticks de cada grupo de la agrupación (p. ej. [2,2,3] en
    // 7/8 produce tres grupos: dos de 2 pulsos y uno de 3).
    std::vector<std::int64_t> groupLengthsInTicks() const;

    // Falso si el numerador/denominador no son positivos, o si la
    // agrupación (cuando se especifica) no suma el numerador.
    bool isValid() const noexcept;

private:
    int numerator_ = 4;
    int denominator_ = 4;
    std::vector<int> grouping_;
};

bool operator==(const TimeSignature& a, const TimeSignature& b);
bool operator!=(const TimeSignature& a, const TimeSignature& b);

// Material musical editable: lista de notas + metadatos. Generadores
// producen Clips, transformadores reciben y devuelven Clips. Una
// instancia del plugin mantiene un único Clip activo a la vez (ver la
// decisión de arquitectura del plan de fases); no existe un "proyecto"
// que contenga a la vez el Clip de Ritmos y el de Armonía.
class Clip
{
public:
    Clip() = default;

    const std::vector<Note>& notes() const noexcept { return notes_; }
    void setNotes(std::vector<Note> notes) { notes_ = std::move(notes); }
    void addNote(const Note& note) { notes_.push_back(note); }

    const TimeSignature& timeSignature() const noexcept { return timeSignature_; }
    void setTimeSignature(TimeSignature ts) { timeSignature_ = std::move(ts); }

    // Tónica en semitonos desde C (0-11) y modo (0=Jónico ... 5=Eólico,
    // 6=Locrio), numerados como los modos griegos sobre la escala mayor.
    int keyRoot() const noexcept { return keyRoot_; }
    void setKeyRoot(int root) noexcept { keyRoot_ = root; }
    int mode() const noexcept { return mode_; }
    void setMode(int mode) noexcept { mode_ = mode; }

    int lengthInBars() const noexcept { return lengthInBars_; }
    void setLengthInBars(int bars) noexcept { lengthInBars_ = bars; }

    std::int64_t lengthInTicks() const;

private:
    std::vector<Note> notes_;
    TimeSignature timeSignature_;
    int keyRoot_ = 0;
    int mode_ = 5;
    int lengthInBars_ = 4;
};

bool operator==(const Clip& a, const Clip& b);
bool operator!=(const Clip& a, const Clip& b);

} // namespace harmonyArchitect::core
