#pragma once

#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Clip.h"
#include "Core/ClipPlayer.h"
#include "Model/ClipState.h"

namespace harmonyArchitect
{

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    // Carga un clip de prueba en el modelo editable; se reemplaza por
    // generación/edición real en el Bloque B. Solo se llama desde el
    // hilo de mensajes (constructor).
    void loadPlaceholderClip();

    // Publica una copia inmutable del modelo editable para que
    // processBlock la lea. Solo se llama desde el hilo de mensajes.
    void publishActiveClip();

    void sendAllNotesOff(juce::MidiBuffer& midiMessages, int sampleOffset);

    // --- Hilo de mensajes ---
    juce::UndoManager undoManager_;
    model::ClipState clipState_{undoManager_};

    // --- Compartido entre hilos ---
    // std::shared_ptr<const Clip>, accedido con std::atomic_load/store:
    // el hilo de audio nunca toca el ValueTree ni el UndoManager, y nunca
    // puede quedarse con un puntero a memoria ya liberada porque el
    // refcount de shared_ptr mantiene viva su copia local hasta soltarla,
    // sin depender de ningún temporizado entre hilos.
    std::shared_ptr<const core::Clip> activeClip_;

    // --- Solo hilo de audio (reservado en prepareToPlay, sin más
    // asignaciones dentro de processBlock) ---
    std::vector<core::PlaybackEvent> playbackEventsScratch_;
    bool wasPlaying_ = false;
    std::int64_t expectedNextTick_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};

} // namespace harmonyArchitect
