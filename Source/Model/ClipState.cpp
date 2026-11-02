#include "ClipState.h"

namespace harmonyArchitect::model
{

namespace
{
const juce::Identifier clipType("Clip");
const juce::Identifier noteType("Note");

const juce::Identifier propTsNum("tsNum");
const juce::Identifier propTsDenom("tsDenom");
const juce::Identifier propTsGrouping("tsGrouping");
const juce::Identifier propKeyRoot("keyRoot");
const juce::Identifier propMode("mode");
const juce::Identifier propLengthBars("lengthBars");

const juce::Identifier propPitch("pitch");
const juce::Identifier propStart("start");
const juce::Identifier propDuration("duration");
const juce::Identifier propVelocity("velocity");
const juce::Identifier propLocked("locked");

juce::String groupingToString(const std::vector<int>& grouping)
{
    juce::StringArray parts;
    for (int g : grouping)
        parts.add(juce::String(g));
    return parts.joinIntoString(",");
}

std::vector<int> groupingFromString(const juce::String& text)
{
    std::vector<int> grouping;
    for (const auto& part : juce::StringArray::fromTokens(text, ",", ""))
        if (part.isNotEmpty())
            grouping.push_back(part.getIntValue());
    return grouping;
}

juce::ValueTree noteToTree(const core::Note& note)
{
    juce::ValueTree noteTree(noteType);
    noteTree.setProperty(propPitch, note.pitch, nullptr);
    noteTree.setProperty(propStart, static_cast<juce::int64>(note.startTicks), nullptr);
    noteTree.setProperty(propDuration, static_cast<juce::int64>(note.durationTicks), nullptr);
    noteTree.setProperty(propVelocity, note.velocity, nullptr);
    noteTree.setProperty(propLocked, note.locked, nullptr);
    return noteTree;
}

core::Note noteFromTree(const juce::ValueTree& noteTree)
{
    core::Note note;
    note.pitch = static_cast<int>(noteTree.getProperty(propPitch, 60));
    note.startTicks = static_cast<juce::int64>(noteTree.getProperty(propStart, 0));
    note.durationTicks = static_cast<juce::int64>(noteTree.getProperty(propDuration, 0));
    note.velocity = static_cast<int>(noteTree.getProperty(propVelocity, 100));
    note.locked = static_cast<bool>(noteTree.getProperty(propLocked, false));
    return note;
}
} // namespace

ClipState::ClipState(juce::UndoManager& undoManager) : undoManager_(undoManager), tree_(clipType)
{
}

void ClipState::loadFrom(const core::Clip& clip)
{
    juce::ValueTree newTree(clipType);

    const auto& ts = clip.timeSignature();
    newTree.setProperty(propTsNum, ts.numerator(), nullptr);
    newTree.setProperty(propTsDenom, ts.denominator(), nullptr);
    newTree.setProperty(propTsGrouping, groupingToString(ts.grouping()), nullptr);
    newTree.setProperty(propKeyRoot, clip.keyRoot(), nullptr);
    newTree.setProperty(propMode, clip.mode(), nullptr);
    newTree.setProperty(propLengthBars, clip.lengthInBars(), nullptr);

    for (const auto& note : clip.notes())
        newTree.appendChild(noteToTree(note), nullptr);

    tree_ = newTree;
}

core::Clip ClipState::toClip() const
{
    core::Clip clip;

    core::TimeSignature ts(static_cast<int>(tree_.getProperty(propTsNum, 4)),
                            static_cast<int>(tree_.getProperty(propTsDenom, 4)),
                            groupingFromString(tree_.getProperty(propTsGrouping, "").toString()));
    clip.setTimeSignature(ts);
    clip.setKeyRoot(static_cast<int>(tree_.getProperty(propKeyRoot, 0)));
    clip.setMode(static_cast<int>(tree_.getProperty(propMode, 5)));
    clip.setLengthInBars(static_cast<int>(tree_.getProperty(propLengthBars, 4)));

    std::vector<core::Note> notes;
    notes.reserve(static_cast<std::size_t>(tree_.getNumChildren()));
    for (const auto& noteTree : tree_)
        notes.push_back(noteFromTree(noteTree));
    clip.setNotes(std::move(notes));

    return clip;
}

void ClipState::addNote(const core::Note& note)
{
    tree_.appendChild(noteToTree(note), &undoManager_);
}

void ClipState::removeNoteAt(int index)
{
    tree_.removeChild(index, &undoManager_);
}

void ClipState::setNoteLocked(int index, bool locked)
{
    tree_.getChild(index).setProperty(propLocked, locked, &undoManager_);
}

} // namespace harmonyArchitect::model
