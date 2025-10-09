# Quick Proto: Pentatonic Note Set

This folder contains a tiny JSON note set consumed by the firmware. As of v0.2-dev you can load these at runtime over Serial — no rebuilds, just paste the JSON blob.

- Scale: C major pentatonic — C D E G A
- MIDI notes: 60, 62, 64, 67, 69
- Serial incantation: `{"notes":[60,62,64,67,69]}` (send with a trailing newline)

Try swapping in other files (e.g., sketch your own `blues.json`) by dragging them onto a serial console that supports paste-send. The firmware will echo back `{"noteset":"loaded", ...}` so you know it stuck.

Included presets:
- `pentatonic.json` — original C major pentatonic.
- `blues.json` — quick A blues flavor mapped around middle C.
