# 1 Quick Proto Note Set

**Goal:** Be playable in 30 minutes with a breadboard + microcontroller.

- **Sensor:** pick one → `optical_reflective` *or* `capacitive_single_wire` *or* `makey_touch`.
- **Mapping:** 1–4 “string zones” mapped to a **C major pentatonic**. 
- **Output:** USB‑MIDI Note On/Off + CC1 (bow/scrape energy).

## Steps
1. Flash `firmware/` default build (Teensy 4.0 env). 
2. Wire the chosen sensor path using `hardware/prototypes/<path>/README.md`.
3. Open a DAW or synth and select a **harp/plucked** patch (or any bright synth).
4. Play: pluck = Note On; relax = Note Off; bow/scrape = CC1 mod depth.

See `examples/quick_proto_note_set/README.md` for the exact scale and the JSON “note set” used by the firmware.
