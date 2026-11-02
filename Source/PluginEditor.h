#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"
#include "UI/HarmonyTab.h"
#include "UI/RhythmTab.h"

namespace harmonyArchitect
{

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor(PluginProcessor& owningProcessor);

    void resized() override;

private:
    juce::TabbedComponent tabs_{juce::TabbedButtonBar::TabsAtTop};
    std::unique_ptr<RhythmTab> rhythmTab_;
    std::unique_ptr<HarmonyTab> harmonyTab_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

} // namespace harmonyArchitect
