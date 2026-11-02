#include "PluginEditor.h"

namespace harmonyArchitect
{

PluginEditor::PluginEditor(PluginProcessor& owningProcessor)
    : AudioProcessorEditor(&owningProcessor)
{
    rhythmTab_ = std::make_unique<RhythmTab>();
    harmonyTab_ = std::make_unique<HarmonyTab>();

    // juce::String(const char*) decodes as ASCII, no UTF-8: los literales con
    // tilde/ñ deben pasar por CharPointer_UTF8 o se corrompen en pantalla.
    tabs_.addTab("Ritmos", juce::Colours::black, rhythmTab_.get(), false);
    tabs_.addTab(juce::String(juce::CharPointer_UTF8("Armonía y Melodía")), juce::Colours::black, harmonyTab_.get(), false);
    addAndMakeVisible(tabs_);

    setSize(1280, 760);
}

void PluginEditor::resized()
{
    tabs_.setBounds(getLocalBounds());
}

} // namespace harmonyArchitect
