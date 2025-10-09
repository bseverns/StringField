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

## Highlights
- **Three sensing paths** (optical / capacitive / MaKey‑style touch) with a common `Sensor` interface.
- **Gesture engine** that names pluck/bow/scrape (and leaves room for harmonics, mutes, slides).
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
- v0.4: OSC bridge & educator packet
- v1.0: Community‑tested kits and lesson plans

> See `docs/Roadmap.md` for dated milestones and test gates.

## Repo conventions
- **Docs‑first:** If it’s not in `docs/`, it’s a rumor.
- **Assumption Ledger:** Update `docs/ASSUMPTION_LEDGER.md` when you lean on a guess.
- **Reproducibility:** Every measurement has a method file next to it.
- **Style:** Comments carry *intent* and *trade‑offs*, not just “what.”
