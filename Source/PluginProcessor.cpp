#include "PluginProcessor.h"

#include <cmath>
#include <memory>

#include "PluginEditor.h"

namespace harmonyArchitect
{

namespace
{
constexpr int kMidiChannel = 1;
}

PluginProcessor::PluginProcessor() : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    loadPlaceholderClip();
    publishActiveClip();
}

void PluginProcessor::loadPlaceholderClip()
{
    // Clip de prueba para verificar la sincronización con el host (Fase
    // 2); se reemplaza por generación/edición real en el Bloque B. 7/8
    // con agrupación 2+2+3, como el preset "Thall" por defecto del
    // mockup -- así el pulso quebrado se nota a simple oído contra el
    // transporte del host.
    core::Clip clip;
    clip.setTimeSignature(core::TimeSignature(7, 8, {2, 2, 3}));
    clip.setLengthInBars(2);

    constexpr int kKick = 36;
    constexpr int kSnare = 38;
    constexpr std::int64_t kEighth = core::TimeSignature::ticksPerQuarterNote / 2;

    auto addHit = [&clip](int pitch, std::int64_t startTicks, int velocity) {
        core::Note note;
        note.pitch = pitch;
        note.startTicks = startTicks;
        note.durationTicks = 200;
        note.velocity = velocity;
        clip.addNote(note);
    };

    const std::int64_t barLength = clip.timeSignature().barLengthInTicks();
    for (int bar = 0; bar < clip.lengthInBars(); ++bar)
    {
        const std::int64_t barStart = static_cast<std::int64_t>(bar) * barLength;
        addHit(kKick, barStart + 0 * kEighth, 110);  // inicio del grupo "2"
        addHit(kKick, barStart + 2 * kEighth, 110);  // inicio del grupo "2"
        addHit(kKick, barStart + 4 * kEighth, 110);  // inicio del grupo "3"
        addHit(kSnare, barStart + 3 * kEighth, 95);
    }

    clipState_.loadFrom(clip);
}

void PluginProcessor::publishActiveClip()
{
    auto newClip = std::make_shared<const core::Clip>(clipState_.toClip());
    std::atomic_store(&activeClip_, newClip);
}

void PluginProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Reserva generosa, de sobra para el contenido de estas fases
    // (patrones de ritmo/armonía): evita que collectPlaybackEvents
    // necesite reasignar memoria dentro de processBlock.
    playbackEventsScratch_.clear();
    playbackEventsScratch_.reserve(1024);

    wasPlaying_ = false;
    expectedNextTick_ = 0;
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::sendAllNotesOff(juce::MidiBuffer& midiMessages, int sampleOffset)
{
    midiMessages.addEvent(juce::MidiMessage::allNotesOff(kMidiChannel), sampleOffset);
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    midiMessages.clear();

    // shared_ptr compartido con el hilo de mensajes: ver el razonamiento
    // de hilos junto a la declaración de activeClip_ en el header.
    const auto clip = std::atomic_load(&activeClip_);

    auto* playHead = getPlayHead();
    const auto position = playHead != nullptr ? playHead->getPosition() : juce::Optional<juce::AudioPlayHead::PositionInfo>{};

    const bool hasPosition = position.hasValue();
    const bool isPlaying = hasPosition && position->getIsPlaying();
    const auto ppq = hasPosition ? position->getPpqPosition() : juce::Optional<double>{};
    const auto bpm = hasPosition ? position->getBpm() : juce::Optional<double>{};

    const bool canPlay = isPlaying && ppq.hasValue() && bpm.hasValue() && *bpm > 0.0 && clip != nullptr
                          && !clip->notes().empty() && clip->lengthInTicks() > 0 && getSampleRate() > 0.0;

    if (!canPlay)
    {
        if (wasPlaying_)
            sendAllNotesOff(midiMessages, 0);
        wasPlaying_ = false;
        return;
    }

    const std::int64_t startTick = static_cast<std::int64_t>(std::llround(*ppq * core::TimeSignature::ticksPerQuarterNote));

    const int numSamples = buffer.getNumSamples();
    const double ticksPerSample = (*bpm) * core::TimeSignature::ticksPerQuarterNote / 60.0 / getSampleRate();
    const std::int64_t rangeLength = std::max<std::int64_t>(1, static_cast<std::int64_t>(std::llround(ticksPerSample * numSamples)));

    // Loop, scrub o reanudar tras un stop: en vez de intentar reconstruir
    // qué debería seguir sonando, apagamos todo y retomamos desde la
    // posición real que reporta el host en este bloque.
    if (!wasPlaying_ || startTick != expectedNextTick_)
        sendAllNotesOff(midiMessages, 0);

    core::collectPlaybackEvents(*clip, startTick, rangeLength, playbackEventsScratch_);

    for (const auto& event : playbackEventsScratch_)
    {
        const int sampleOffset =
            juce::jlimit(0, numSamples - 1, static_cast<int>(std::llround(static_cast<double>(event.tickInRange) / ticksPerSample)));

        const auto message = event.isNoteOn ? juce::MidiMessage::noteOn(kMidiChannel, event.pitch, (juce::uint8)event.velocity)
                                             : juce::MidiMessage::noteOff(kMidiChannel, event.pitch);
        midiMessages.addEvent(message, sampleOffset);
    }

    wasPlaying_ = true;
    expectedNextTick_ = startTick + rangeLength;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

bool PluginProcessor::hasEditor() const
{
    return true;
}

const juce::String PluginProcessor::getName() const
{
    return "Harmony Architect";
}

bool PluginProcessor::acceptsMidi() const
{
    return true;
}

bool PluginProcessor::producesMidi() const
{
    return true;
}

bool PluginProcessor::isMidiEffect() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int /*index*/)
{
}

const juce::String PluginProcessor::getProgramName(int /*index*/)
{
    return {};
}

void PluginProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/)
{
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& /*destData*/)
{
}

void PluginProcessor::setStateInformation(const void* /*data*/, int /*sizeInBytes*/)
{
}

} // namespace harmonyArchitect

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new harmonyArchitect::PluginProcessor();
}
