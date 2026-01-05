# StringField

_A spatial/gestural string‑like interface for pluck/bow/scrape metaphors._

> Built to be played, rebuilt to be understood. This repo is a studio notebook: code, circuits, notes, and tests in one place. It’s designed to let others **create** with it—quickly, critically, and together.

## What this is

StringField explores the sensation of playing an “imaginary string” stretched in space. The “string” is not a single technology; it’s a family of **sensor stacks** (optical / capacitive / touch‑to‑ground via MaKey‑MaKey, etc.) and **gesture detectors** (pluck, bow, scrape) that translate motion + intent into **MIDI/OSC** and/or **audio**.

- **Pedagogy first:** everything is annotated and meant to be teachable.
- **Modular by design:** swap sensing methods without rewriting mappings.
- **Ethical by default:** privacy‑preserving, repairable, locally sourceable.

## Quick Map

| Path | What’s inside |
| --- | --- |
| `docs/` | Design briefs, play paradigms, sensing survey, assumption ledger, roadmap, **Touch-to-Ground Tuning Kit**, **Classroom Code Tour** |
| `firmware/` | PlatformIO project(s). Teensy 4.0 by default; optional ESP32 target |
| `software/` | Processing + p5.js visualizers and bridges (now with gesture debugger) |
| `hardware/` | Prototype notes per sensing modality + example BOM stubs |
| `examples/` | “1 quick proto note set” and small demos |
| `tools/` | Utilities (serial logger, calibration helpers) |
| `.github/` | CI to build firmware & lint docs |

## CI / test beacons

- **Gesture brain tested on-host:** `pio test -d firmware -e native` runs Unity cases for the pluck/bow/scrape/harmonic/vibrato heuristics.
- **Portability check:** CI builds Teensy 4.0 _and_ ESP32-S3 targets to prove the sensor abstraction is real, not aspirational.
- **Formatting sanity:** Prettier watches docs + p5.js sketches; `clang-format` keeps the Processing projector viz from rotting.

## Build kit + educator packet

Want the turnkey, print-and-go version? See [`docs/BuildKit.md`](docs/BuildKit.md) for the finalized BOM, wiring snapshots per sensor stack, flashing steps, calibration micro-rituals, and classroom handouts + first-play lesson plan.

## Runtime note-set auditions (Serial preset browser)

When you hear someone shout “try it in Hirajōshi!”, you no longer have to recompile.

1. Open a serial terminal at `115200` baud (Teensy shows up as `/dev/ttyACM*`, etc.).
2. Paste a JSON blob like `{"notes":[60, 63, 67, 70, 74]}` and hit return.
3. The firmware cuts any ringing note, loads the new scale, and echoes back the set as JSON.

The Processing/p5.js visualizers understand these same gesture packets, so you can narrate what changed in real time while the class hears it.

## Touch-to-ground tuning kit

The new field guide lives at [`docs/TouchGroundTuningKit.md`](docs/TouchGroundTuningKit.md). It walks through humid vs. dry room RC combos, flowcharts you can literally read aloud, and narration prompts tying gestures to MIDI semantics.

## Classroom code tour

Need to narrate the firmware without scrolling through 400 lines live? Print or tab over to [`docs/ClassroomCodeTour.md`](docs/ClassroomCodeTour.md). It mirrors the in-file comments and gives you quick bullet points to read while the class traces sensor → gesture → MIDI mappings.

## New gesture palette + calibration cheatsheet

These additions are meant to be taught live—think of this as a lab whiteboard made text:

- **Harmonics:** light touches (0.35–0.65 normalized) held for ~70 ms become octave-up, glassy notes. The GestureEngine now watches for _stillness_ (`harmonic_variation_eps`) so the class can hear the moment a touch steadies.
- **Mutes:** quick, low-energy touches (<50 ms or <0.25 peak) cut the sustaining note and emit a short telemetry blip. A falling envelope below `mute_release_thresh` arms the mute so students see why a brush counts as damping.
- **Tremolo/Vibrato:** fast amplitude sign flips map to CC11 (expression) or pitch bend. Shallow wobble depth → tremolo; deeper swing → vibrato. A short `tremolo_grace_us` ignores jitter on the very first attack so you can narrate the motion separately from the pluck.

Calibration micro-rituals (two minutes per class):

1. **Set the contact floor/ceiling** using `on_thresh` / `off_thresh` while watching the Serial feed. Avoid chatter.
2. **Sweep the light touch band**: lightly graze the sensor and nudge `harmonic_peak_min`/`max` and `harmonic_variation_eps` until harmonics trigger without full plucks.
3. **Mute window sanity**: tap and lift quickly; adjust `mute_window_us` and `mute_release_thresh` if students have slow reflexes or the room hums.
4. **Wobble feel**: shake your finger above the sensor; set `tremolo_min_delta`, `tremolo_grace_us`, and `wobble_goal` so tremolo happens at intentional oscillations, not random noise.

## Sensor stack expansions

- **Time-of-flight (ToF):** drop in a VL53L0X/TMF8801 board, feed its analog/filtered output to `A2`, and compile with `-D SENSOR_TOF`. The sensor class now clamps the floor for noisy rooms and keeps the pin-only setup notes inline; swap in a real I²C driver later without touching `GestureEngine`.
- **Piezo contact mic:** bias a piezo disc with a megaohm resistor, clamp the extremes, and plug into `A3` with `-D SENSOR_PIEZO`. The class can _see_ hits via the onboard LED and adjust the `bias` smoothing if the room hum drifts.
- **PIR motion:** run the PIR gate to a digital pin (`-D SENSOR_PIR`). We purposely wait out the 30s warm-up, then smooth the binary gate into a motion envelope so students can _see_ lingering activity instead of a jittery square wave.
- **Electret mic (analog):** a bias resistor + RC envelope into `A4` (`-D SENSOR_ELECTRET`). The class follows bias slowly, rectifies the swing, and exposes a gain knob so you can narrate why the whisper floor is clamped where it is.
- **I²S/PDM mic:** for SPH0645/ICS-43434/etc., compile with `-D SENSOR_I2S_MIC`. The stub reads short bursts off I²S, averages absolute amplitude, and outputs a mellow envelope so the rest of the firmware doesn’t care which mic showed up.

## Highlights

- **Three (now five) sensing paths** (optical / capacitive / MaKey‑style touch / time‑of‑flight proximity / piezo contact mic) with a common `Sensor` interface.
- **Gesture engine** that names pluck/bow/scrape plus harmonics, mutes, tremolo, and vibrato heuristics that are easy to narrate and tune.
- **Runtime scale swaps** via JSON over Serial — send `{"notes":[...]}` and audition without reflashing.
- **Gesture debugger views** in Processing + p5.js with velocity vectors and state tickers, projector-ready.
- **MIDI first** (USB MIDI + 5‑pin / TRS as needed), OSC optional via serial bridge.
- **Calibration discipline:** reproducible steps, saved profiles, and an Assumption Ledger.

## Ethics (what we optimize for)

- **Access & repair:** commodity parts, publish alternates & cost ranges.
- **Agency:** mappings are visible, editable, and documented; runtime tuning where possible.
- **Privacy:** no hidden data collection; everything runs offline; explicit consent before any logging.
- **Safety:** low‑voltage, insulated touch paths; conservative thresholds; clear disclaimers.

## Roadmap (living doc)

- v0.1: Quick‑proto playable (pentatonic), one sensor path, minimal viz
- v0.2: Swap sensor paths without changing mappings (single interface)
- v0.3: Bow detection + tremolo/vibrato heuristics
- v0.4: OSC bridge & educator packet (projector overlays, consent-first logger strip, printable handouts — see [`docs/OSCSerialBridge.md`](docs/OSCSerialBridge.md) + [`docs/BridgeLessonHandouts.md`](docs/BridgeLessonHandouts.md))
- v1.0: Community‑tested kits and lesson plans

> See `docs/Roadmap.md` for dated milestones and test gates.

## What to expand next (punk‑rock wishlist)

Three concrete expansions we’d love to see land — each one keeps the “studio notebook + teaching guide” ethos intact:

- **Deepen the gesture vocabulary:** add classroom‑narratable heuristics for harmonics, mutes, and tremolo/vibrato, with calibration micro‑rituals written right next to the code and echoed in docs.
- **Grow the sensor front‑ends:** promote ToF, piezo, PIR, electret, and I²S mic paths from stubs into fully documented `Sensor` modules with wiring notes, tuning tips, and failure modes.
- **Build the educator bridge:** ship the OSC/Serial bridge + lesson packet as a turnkey, print‑and‑perform tool so workshops can run without improvising glue code.

## Repo conventions

- **Docs‑first:** If it’s not in `docs/`, it’s a rumor.
- **Assumption Ledger:** Update `docs/ASSUMPTION_LEDGER.md` when you lean on a guess.
- **Reproducibility:** Every measurement has a method file next to it.
- **Style:** Comments carry _intent_ and _trade‑offs_, not just “what.”
