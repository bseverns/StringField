# Electret Mic (Analog Envelope)

Electret mics are cheap and expressive. They catch **air motion**—scrapes, breath, finger flicks, and tiny percussive taps.

## Wiring notes

- **Firmware path:** `firmware/src/electret_mic_sensor.cpp` (`SENSOR_ELECTRET`)
- **Signal pin:** `A4`
- You need a **bias resistor** and usually a **rectifier/envelope** stage.
- Keep the wires short; this is an antenna.

**Sketch wiring (minimal):**

- Electret mic → bias resistor (2.2 kΩ–10 kΩ) to 3.3 V.
- Coupling cap (1–10 µF) into a diode + RC envelope (see below) → `A4`.
- GND to GND.

## Recommended RC values

- **Bias resistor:** **2.2 kΩ–10 kΩ** (higher = lower current).
- **Envelope:** **10 kΩ + 1 µF** (≈10 ms) for quick plucks.
- For slower swells, try **100 kΩ + 1 µF** (≈100 ms).

## Calibration steps (live, out loud)

1. **Idle bias check:** the normalized value should hover near 0.0 after the bias tracker settles.
2. **Gain tuning:** increase `gain_` until quiet room noise is visible but not pegged.
3. **Floor clamp:** if the room hums, raise the `env_` floor clamp so idle reads 0.

## Expected gesture behavior

- **Pluck:** snap or tap near the mic for quick spikes.
- **Scrape:** steady rubbing makes a noisy plateau—great for texture.
- **Bow:** breath or slow movement can create soft, airy sustains.

## Common failure modes

- **DC offset drift:** missing/incorrect bias resistor causes wandering baselines.
- **Hum city:** long wires pick up 50/60 Hz; twist or shield.
- **Perma‑clip:** gain too hot or envelope too fast so everything is 1.0.
