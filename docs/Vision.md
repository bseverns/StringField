# Vision

**Goal:** A playable, swappable “string in space” that invites pluck/bow/scrape gestures and speaks MIDI/OSC. The system should be small, cheap, and robust enough for workshops, yet deep enough for performance and research.

**Constraints**
- Classroom‑proof: 15‑minute setup; no laptop admin rights required.
- Off‑grid friendly: all core functions offline; logging opt‑in and obvious.
- Repairable: local parts, through‑hole where feasible for EDU kits.

**Design Vectors**
- **Sensing**: optical (reflective IR / time‑of‑flight), capacitive (single‑wire + guard), touch‑to‑ground (MaKey‑style), piezo, hall/flex (later).
- **Gesture**: pluck onset, bow direction + speed, scrape granularity; later: mutes, harmonics, tremolo/vibrato.
- **Mapping**: scale‑aware note selection; continuous timbre axes; pressure/proximity → CCs.
