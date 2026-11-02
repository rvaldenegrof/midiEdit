#include <juce_core/juce_core.h>

#include "Core/Clip.h"

using namespace harmonyArchitect::core;

namespace
{

class TimeSignatureTests : public juce::UnitTest
{
public:
    TimeSignatureTests() : juce::UnitTest("TimeSignature", "Core") {}

    void runTest() override
    {
        beginTest("4/4 regular: longitud de compas y agrupacion por defecto");
        {
            const TimeSignature ts(4, 4);
            expectEquals(ts.barLengthInTicks(), static_cast<std::int64_t>(4 * TimeSignature::ticksPerQuarterNote));

            const auto groups = ts.groupLengthsInTicks();
            expectEquals(static_cast<int>(groups.size()), 4);
            for (auto g : groups)
                expectEquals(g, static_cast<std::int64_t>(TimeSignature::ticksPerQuarterNote));

            expect(ts.isValid());
        }

        beginTest("7/8 con agrupacion 2+2+3 (thall/djent quebrado)");
        {
            const TimeSignature ts(7, 8, {2, 2, 3});
            const std::int64_t eighth = TimeSignature::ticksPerQuarterNote / 2;
            expectEquals(ts.barLengthInTicks(), 7 * eighth);

            const auto groups = ts.groupLengthsInTicks();
            expectEquals(static_cast<int>(groups.size()), 3);
            expectEquals(groups[0], 2 * eighth);
            expectEquals(groups[1], 2 * eighth);
            expectEquals(groups[2], 3 * eighth);

            expect(ts.isValid());
        }

        beginTest("11/8 con agrupacion que no suma el numerador es invalida");
        {
            const TimeSignature ts(11, 8, {2, 2, 3});
            expect(!ts.isValid());
        }

        beginTest("Numerador o denominador no positivos son invalidos");
        {
            expect(!TimeSignature(0, 4).isValid());
            expect(!TimeSignature(4, 0).isValid());
        }

        beginTest("Igualdad e desigualdad");
        {
            expect(TimeSignature(7, 8, {2, 2, 3}) == TimeSignature(7, 8, {2, 2, 3}));
            expect(TimeSignature(7, 8, {2, 2, 3}) != TimeSignature(7, 8, {3, 2, 2}));
            expect(TimeSignature(7, 8) != TimeSignature(11, 8));
        }
    }
};

static TimeSignatureTests timeSignatureTests;

} // namespace
