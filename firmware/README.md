# Firmware field notes + test ritual

> We keep the firmware like a studio notebook: scribbles about intent next to runnable code. Swap sensors, run the behavior tests, then flash two targets to prove nothing is hardwired to one MCU.

## What lives here
- `platformio.ini` with Teensy 4.0 + ESP32-S3 environments and a `native` test target that only builds the gesture brain.
- `src/main.cpp` for hardware glue + MIDI mapping and `src/gesture_engine.cpp` for the sensor-agnostic gesture state machine.
- `test/test_gesture_engine/` with Unity cases that beat on the pluck/bow/scrape/vibrato transitions so students can see the rules.

## Build / test quickstart
Run these from the repo root:

```bash
# Behavior tests on the host (no hardware). Exercises GestureEngine timing + hysteresis.
pio test -d firmware -e native

# Firmware builds proving portability across sensor stacks + MCUs.
pio run -d firmware -e teensy40
pio run -d firmware -e esp32s3
```

If you add a new sensor path, keep its `SensorSample` output normalized 0..1 and timestamped in microseconds; the tests will catch regressions in the gesture transitions.

## CI and formatting
- CI runs the native Unity suite, then builds Teensy and ESP32 artifacts to prove the abstraction holds.
- Docs + p5.js sketches are checked with Prettier; the Processing sketch runs through `clang-format --dry-run` to keep projector demos tidy.

Keep comments that explain why the knob exists, not just what it does—this code is meant to be read aloud.
