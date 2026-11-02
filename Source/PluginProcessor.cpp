#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace harmonyArchitect
{

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void PluginProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/)
{
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    // Fase 0: sin modelo ni generación todavía. No hay síntesis de audio
    // propia (el plugin solo emite MIDI en fases posteriores).
    buffer.clear();
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
