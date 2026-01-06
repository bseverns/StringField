# Capacitive Single‑Wire (Field Notes)

_Goal: make an invisible string using a single wire + grounding ritual. This modality is sensitive and expressive, but it **demands** a good ground story._

## What it feels like

Capacitive single‑wire sensing is **ghost‑string** mode: the player doesn’t have to touch a pad; just approaching the wire changes the capacitance. It’s soft, quiet, and beautifully expressive—but it’s also moody when humidity shifts or grounding is sloppy.

---

## Wiring sketch (prototype‑level)

- **Sensor wire:** a long lead or foil strip to `A1`
- **Ground reference:** wearable bracelet or a shared foil pad to GND
- **No fancy IC required:** this is the simplest RC‑rise trick (read the pin, discharge it, charge it, read it again)

**Expected signal range:** analog 0..1023 (ADC).  
**Firmware file:** `firmware/src/capacitive_sensor.cpp`

---

## Calibration ritual (4 minutes)

1. **Check the baseline:** no one touches the wire. You should see a stable, low signal.
2. **Hover and touch:** approach the wire, then touch. The delta should grow.
3. **Adjust sensitivity:** tweak `sensitivity_scale_` (in `firmware/src/capacitive_sensor.cpp`) until the envelope hits 0.7–0.9 on intentional touches but stays near 0 at rest.

---

## Narratable failure modes

- **Always maxed out:** missing or bad ground. The body is not part of the RC circuit, so the pad floats high.
- **Flatline:** wire broken or pin mis‑mapped; verify `A1`.
- **Noisy crawl:** humidity swings. Lower the baseline follower rate or re‑run calibration mid‑session.

---

## Teach the normalization

Say this:

> “We’re not measuring distance directly. We’re measuring how long a tiny RC circuit takes to charge. Then we map that to 0..1 so the gesture engine doesn’t care what the sensor is.”

If students get that, they can debug it.
