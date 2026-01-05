# Sensor Field Notes Index

_This folder is half studio notebook, half teach‑from‑the‑bench script._ Each page is a **field note** you can read aloud while wiring, calibrating, and demoing the gesture vocabulary. Expect wiring sketches, RC values, calibration rituals, and what the gestures should _feel_ like.

If you want the fastest path: pick a sensor below, wire it, compile with the matching `-D SENSOR_*` flag, and narrate the calibration steps as you turn the knobs.

## The sensor menu

- **MaKey‑style touch‑to‑ground:** [`MakeyTouch.md`](MakeyTouch.md)
- **Time‑of‑flight proximity:** [`TimeOfFlight.md`](TimeOfFlight.md)
- **Piezo / contact mic:** [`Piezo.md`](Piezo.md)
- **PIR motion gate:** [`PIR.md`](PIR.md)
- **Electret mic (analog envelope):** [`ElectretMic.md`](ElectretMic.md)
- **I²S / PDM digital mic:** [`I2SMic.md`](I2SMic.md)

## How to teach with these pages

1. **Point at the expected signal range** (analog 0–1023, digital gate, PCM, etc.).
2. **Describe normalization** (how we map whatever the sensor gives us into 0..1).
3. **Call out failure modes** (and how to debug them live).
4. **Play the gestures** (pluck / bow / scrape) and compare how each sensor “tells the story.”

That’s the whole vibe: tech as performance, troubleshooting as collaboration.
