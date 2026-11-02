#include <juce_core/juce_core.h>

// Corre todos los juce::UnitTest registrados (Core/Model) sin levantar
// ningún host ni ventana. Sale con código distinto de cero si algo falla,
// para poder usarse en CI.
int main()
{
    juce::UnitTestRunner runner;
    runner.setAssertOnFailure(false);
    runner.runAllTests();

    int numFailures = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
        numFailures += runner.getResult(i)->failures;

    return numFailures == 0 ? 0 : 1;
}
