# Harmony Architect

Plugin VST3 (C++/JUCE) para REAPER en Windows, orientado a componer música original (thall/djent y géneros afines) aplicando **formas de composición** sobre material MIDI editable.

## Objetivo

- Generar y transformar material musical mediante reglas, y poder editarlo siempre a mano.
- Todo resultado es un **clip MIDI editable**, ya sea generado, escrito por el usuario o transformado.
- Dos pestañas principales:
  1. **Ritmos**: patrones de percusión/ritmo en rejilla (compases irregulares, polirritmia, euclídeos).
  2. **Armonía y melodía**: progresiones, voicings y motivos con técnicas de desarrollo.
- Editor tipo **piano roll** compartido por ambas pestañas (modo percusión y modo melódico).

## Stack

- C++17 o superior, JUCE 8, CMake.
- Formato: VST3 (instrumento con salida MIDI). Host de prueba: REAPER.
- IDE: VS Code con extensiones CMake Tools y C/C++ (o clangd).
- Compilador en Windows: MSVC (Visual Studio Build Tools 2022).

## Estructura propuesta

```
HarmonyArchitect/
├── CMakeLists.txt
├── CLAUDE.md
├── third_party/
│   └── JUCE/                  # submódulo git o FetchContent
├── Source/
│   ├── PluginProcessor.h/.cpp # AudioProcessor, salida MIDI, transporte
│   ├── PluginEditor.h/.cpp    # Ventana principal con TabbedComponent
│   ├── Core/                  # Teoría musical y datos (SIN dependencias de UI)
│   │   ├── Clip.h/.cpp        # Clip, Note, TimeSignature
│   │   ├── Scale.h/.cpp
│   │   ├── Chord.h/.cpp
│   │   ├── Generators/        # Euclídeo, progresiones, motivos
│   │   └── Transforms/        # Rotación, inversión, retrogradación, etc.
│   ├── Model/                 # ValueTree, UndoManager, serialización
│   └── UI/
│       ├── PianoRollComponent.h/.cpp
│       ├── RhythmTab.h/.cpp
│       └── HarmonyTab.h/.cpp
└── Tests/                     # Pruebas de Core (sin host)
```

## Decisiones de arquitectura

- **Core independiente de la UI y de JUCE cuando sea posible**: teoría musical, generadores y transformadores se pueden testear sin abrir un host.
- **Un único modelo de clip**: `Clip` = lista de notas + metadatos (compás, tonalidad). Generadores producen clips, transformadores reciben y devuelven clips.
- **Unidades en ticks** (960 PPQ). Nunca guardar segundos ni píxeles en el modelo; los píxeles se calculan solo al dibujar.
- **Vista separada de datos**: el piano roll lee y modifica el `ValueTree` mediante comandos con `UndoManager`; no guarda estado propio de notas.
- **Notas bloqueables**: al regenerar, solo cambian las notas desbloqueadas.
- **Cadena de transformaciones no destructiva**: clip original + lista de transformaciones, con opción de "aplicar" para volverlo clip plano.
- **Semilla (seed)** por generador para resultados reproducibles.
- **Hilos**: la UI edita en el hilo de mensajes; `processBlock` lee una copia inmutable o un buffer lock-free. Nunca compartir el `ValueTree` directamente con el hilo de audio, ni asignar memoria ni bloquear dentro de `processBlock`.
- **Compases irregulares** (7/8, 11/8, 5/4…) soportados desde el inicio; las líneas del piano roll salen del `TimeSignature` del clip.
- **Exportar `.mid`** como respaldo si el host no permite arrastrar clips desde el plugin.

## Compilar

Requisitos: Visual Studio Build Tools 2022 (carga de trabajo C++), CMake, Git.

```bash
git submodule add https://github.com/juce-framework/JUCE.git third_party/JUCE
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

El `.vst3` queda en `build/HarmonyArchitect_artefacts/Release/VST3/`. Copiarlo a `C:\Program Files\Common Files\VST3\` (o activar `COPY_PLUGIN_AFTER_BUILD` en CMake) y reescanear plugins en REAPER.

Opciones CMake relevantes en `juce_add_plugin`:

- `IS_SYNTH TRUE`
- `NEEDS_MIDI_INPUT TRUE`
- `NEEDS_MIDI_OUTPUT TRUE`
- `FORMATS VST3 Standalone` (el Standalone sirve para depurar la UI sin REAPER)

## Convenciones de código

- Nombres: clases `PascalCase`, métodos y variables `camelCase`, miembros privados con sufijo `_` o prefijo consistente (elegir uno y mantenerlo).
- Usar `juce::` explícito; evitar `using namespace juce` en headers.
- RAII y smart pointers (`std::unique_ptr`, `juce::ReferenceCountedObjectPtr`); sin `new`/`delete` manuales.
- Marcar `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR` en los componentes.
- Todo lo musical (teoría, generadores, transformaciones) con pruebas unitarias en `Tests/`.
- Comentarios y documentación del proyecto en español; identificadores en inglés.

## Hoja de ruta

1. **Esqueleto**: proyecto CMake + JUCE, plugin que carga en REAPER, `TabbedComponent` con dos pestañas vacías.
2. **Modelo**: `Clip`, `Note`, serialización con `ValueTree`, `UndoManager`.
3. **Salida MIDI**: reproducir un clip sincronizado con el transporte del host (`AudioPlayHead`).
4. **Piano roll básico**: dibujar, mover, redimensionar y borrar notas, con snap y zoom.
5. **Pestaña Ritmos**: rejilla de percusión + generador euclídeo + compases irregulares.
6. **Transformadores**: rotación, inversión, retrogradación, aumentación/disminución.
7. **Pestaña Armonía/Melodía**: escalas y modos, progresiones, voicings, motivos y desarrollo motívico.
8. **Pulido**: bloqueo de notas, variaciones/historial, ghost de transformaciones, exportación `.mid`, presets.

## Cómo trabajar con Claude en este repositorio

- Antes de añadir una función musical, ponerla en `Core/` con su prueba, y recién después conectarla a la UI.
- Mantener los cambios pequeños y compilables; una fase de la hoja de ruta a la vez.
- No introducir dependencias nuevas sin discutirlo.
- Si una decisión toca el modelo de datos (`Clip`, `ValueTree`) o los hilos, explicar el razonamiento antes de implementar.
