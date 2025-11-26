# StringField

*A spatial/gestural string‑like interface for pluck/bow/scrape metaphors.*

> Built to be played, rebuilt to be understood. This repo is a studio notebook: code, circuits, notes, and tests in one place. It’s designed to let others **create** with it—quickly, critically, and together.

## What this is
StringField explores the sensation of playing an “imaginary string” stretched in space. The “string” is not a single technology; it’s a family of **sensor stacks** (optical / capacitive / touch‑to‑ground via MaKey‑MaKey, etc.) and **gesture detectors** (pluck, bow, scrape) that translate motion + intent into **MIDI/OSC** and/or **audio**.

- **Pedagogy first:** everything is annotated and meant to be teachable.
- **Modular by design:** swap sensing methods without rewriting mappings.
- **Ethical by default:** privacy‑preserving, repairable, locally sourceable.

## Quick Map
Path | What’s inside
---|---
`docs/` | Design briefs, play paradigms, sensing survey, assumption ledger, roadmap, **Touch-to-Ground Tuning Kit**, **Classroom Code Tour**
`firmware/` | PlatformIO project(s). Teensy 4.0 by default; optional ESP32 target
`software/` | Processing + p5.js visualizers and bridges (now with gesture debugger)
`hardware/` | Prototype notes per sensing modality + example BOM stubs
`examples/` | “1 quick proto note set” and small demos
`tools/` | Utilities (serial logger, calibration helpers)
`.github/` | CI to build firmware & lint docs

## Runtime note-set auditions (Serial preset browser)
When you hear someone shout “try it in Hirajōshi!”, you no longer have to recompile.

1. Open a serial terminal at `115200` baud (Teensy shows up as `/dev/ttyACM*`, etc.).
2. Paste a JSON blob like `{"notes":[60, 63, 67, 70, 74]}` and hit return.
3. The firmware cuts any ringing note, loads the new scale, and echoes back the set as JSON.

The Processing/p5.js visualizers understand these same gesture packets, so you can narrate what changed in real time while the class hears it.

## Touch-to-ground tuning kit
The new field guide lives at [`docs/TouchGroundTuningKit.md`](docs/TouchGroundTuningKit.md). It walks through humid vs. dry room RC combos, flowcharts you can literally read aloud, and narration prompts tying gestures to MIDI semantics.

## Classroom code tour
Need to narrate the firmware without scrolling through 400 lines live? Print or
tab over to [`docs/ClassroomCodeTour.md`](docs/ClassroomCodeTour.md). It mirrors
the in-file comments and gives you quick bullet points to read while the class
traces sensor → gesture → MIDI mappings.

## New gesture palette + calibration cheatsheet
These additions are meant to be taught live—think of this as a lab whiteboard made text:

- **Harmonics:** light touches (0.35–0.65 normalized) held for ~70 ms become octave-up, glassy notes. Coach students to hover until the debug print says `harmonic` then lift.
- **Mutes:** quick, low-energy touches (<50 ms or <0.25 peak) cut the sustaining note and emit a short telemetry blip. Great for call-and-response damping drills.
- **Tremolo/Vibrato:** fast amplitude sign flips map to CC11 (expression) or pitch bend. Shallow wobble depth → tremolo; deeper swing → vibrato. Tune `tremolo_min_delta`, `tremolo_max_period_us`, and `vibrato_depth_min` during rehearsal.

Calibration micro-rituals (two minutes per class):
1. **Set the contact floor/ceiling** using `on_thresh` / `off_thresh` while watching the Serial feed. Avoid chatter.
2. **Sweep the light touch band**: lightly graze the sensor and nudge `harmonic_peak_min`/`max` until harmonics trigger without full plucks.
3. **Mute window sanity**: tap and lift quickly; adjust `mute_window_us` if students have slow reflexes.
4. **Wobble feel**: shake your finger above the sensor; set `tremolo_min_delta` and `wobble_goal` so tremolo happens at intentional oscillations, not random noise.

## Sensor stack expansions
- **Time-of-flight (ToF):** drop in a VL53L0X/TMF8801 board, feed its analog/filtered output to `A2`, and compile with `-D SENSOR_TOF`. The stub maps the depth envelope to 0..1 with a slightly faster low-pass to catch hand waves. Swap in a real I²C driver later without touching `GestureEngine`.
- **Piezo contact mic:** bias a piezo disc with a megaohm resistor, clamp the extremes, and plug into `A3` with `-D SENSOR_PIEZO`. The class can *see* hits via the onboard LED and adjust the `bias` smoothing if the room hum drifts.

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
- v0.4: OSC bridge & educator packet (see [`docs/OSCSerialBridge.md`](docs/OSCSerialBridge.md) + [`docs/BridgeLessonHandouts.md`](docs/BridgeLessonHandouts.md))
- v1.0: Community‑tested kits and lesson plans

> See `docs/Roadmap.md` for dated milestones and test gates.

## Repo conventions
- **Docs‑first:** If it’s not in `docs/`, it’s a rumor.
- **Assumption Ledger:** Update `docs/ASSUMPTION_LEDGER.md` when you lean on a guess.
- **Reproducibility:** Every measurement has a method file next to it.
- **Style:** Comments carry *intent* and *trade‑offs*, not just “what.”
