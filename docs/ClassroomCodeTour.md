# Classroom Code Tour

_Read this aloud with your class._ These notes map source files to the gestures and stories they teach. Everything is annotated in the code, but this doc keeps the narrative in one place when you need to riff without scrolling.

## Firmware (`firmware/src/main.cpp`)

- **Sensor abstraction:** Look for the `Sensor` base class and the three demo implementations (`OpticalSensor`, `CapacitiveSensorDemo`, `MakeySensorDemo`). Each has paragraph-level comments that spell out what the pins do and how raw data gets normalized.
- **Gesture engine:** The `GestureEngine` section is a manifesto on thresholds, hysteresis, and timing. Every variable is commented with the trade-off it represents so students can tweak constants and immediately see the effect.
- **Serial preset browser:** Functions like `parse_note_set_json` and `pump_serial_commands` have doc blocks that narrate error handling, buffer limits, and why we echo acknowledgements. Show the class how this keeps the firmware robust without a heavy JSON library.
- **Loop narration:** `setup()` and `loop()` carry docstrings that mirror the performative steps (sense → classify → map → narrate). Use those as chapter headings in your lesson.

## Processing + p5.js visualizers (`software/processing`, `software/p5js`)

- **Function-by-function glossaries:** Every drawing function now explains what it renders and how it ties back to the instrument ("vector arrow matches MIDI velocity" etc.).
- **Keyboard rehearsal cheats:** Comments before `keyPressed()` in both versions make it explicit which keys to tap when the hardware is still being wired.
- **History + noise layers:** Notes describe why we add gritty textures and short gesture tickers—use them to discuss legibility and feedback design.

## Docs + Field Guides

- **Touch Ground Tuning Kit:** Already reads like a zine—pair it with the new comments so hardware students know exactly where to apply resistor/cap tweaks.
- **Prototype Note Sets:** Updated README references the blues preset and the JSON schema kids can remix.

## How to keep the commentary fresh

1. **Treat comments as living narration.** After every rehearsal, tweak the code comments with what the class asked. That keeps the repo honest.
2. **Cross-link patches.** When you add a new gesture, put a two-line summary in both the firmware file and this doc so future educators can find it.
3. **Invite annotation.** Students can PR their own comments—it's an act of reflective practice.

> Punk-rock reminder: code is literature. Write it loud enough that someone in the back of the room can shout along.
