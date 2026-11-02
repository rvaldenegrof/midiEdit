#include "Clip.h"

#include <numeric>

namespace harmonyArchitect::core
{

bool operator==(const Note& a, const Note& b)
{
    return a.pitch == b.pitch && a.startTicks == b.startTicks && a.durationTicks == b.durationTicks
           && a.velocity == b.velocity && a.locked == b.locked;
}

bool operator!=(const Note& a, const Note& b)
{
    return !(a == b);
}

TimeSignature::TimeSignature(int numerator, int denominator) : numerator_(numerator), denominator_(denominator)
{
}

TimeSignature::TimeSignature(int numerator, int denominator, std::vector<int> grouping)
    : numerator_(numerator), denominator_(denominator), grouping_(std::move(grouping))
{
}

std::int64_t TimeSignature::barLengthInTicks() const
{
    const std::int64_t ticksPerBeat = 4 * static_cast<std::int64_t>(ticksPerQuarterNote) / denominator_;
    return static_cast<std::int64_t>(numerator_) * ticksPerBeat;
}

std::vector<std::int64_t> TimeSignature::groupLengthsInTicks() const
{
    const std::int64_t ticksPerBeat = 4 * static_cast<std::int64_t>(ticksPerQuarterNote) / denominator_;
    std::vector<std::int64_t> result;

    if (grouping_.empty())
    {
        result.assign(static_cast<std::size_t>(numerator_), ticksPerBeat);
    }
    else
    {
        result.reserve(grouping_.size());
        for (int beatsInGroup : grouping_)
            result.push_back(static_cast<std::int64_t>(beatsInGroup) * ticksPerBeat);
    }

    return result;
}

bool TimeSignature::isValid() const noexcept
{
    if (numerator_ <= 0 || denominator_ <= 0)
        return false;

    if (grouping_.empty())
        return true;

    const int groupSum = std::accumulate(grouping_.begin(), grouping_.end(), 0);
    return groupSum == numerator_;
}

bool operator==(const TimeSignature& a, const TimeSignature& b)
{
    return a.numerator() == b.numerator() && a.denominator() == b.denominator() && a.grouping() == b.grouping();
}

bool operator!=(const TimeSignature& a, const TimeSignature& b)
{
    return !(a == b);
}

std::int64_t Clip::lengthInTicks() const
{
    return static_cast<std::int64_t>(lengthInBars_) * timeSignature_.barLengthInTicks();
}

bool operator==(const Clip& a, const Clip& b)
{
    return a.notes() == b.notes() && a.timeSignature() == b.timeSignature() && a.keyRoot() == b.keyRoot()
           && a.mode() == b.mode() && a.lengthInBars() == b.lengthInBars();
}

bool operator!=(const Clip& a, const Clip& b)
{
    return !(a == b);
}

} // namespace harmonyArchitect::core
