# PIR Motion Sensor

PIR is the “big‑gesture” detector: it doesn’t see fingers, it sees **body heat motion**. Great for dramatic bows, stage‑wide gesturing, and “presence” cues.

## Wiring notes

- **Firmware path:** `firmware/src/pir_sensor.cpp` (`SENSOR_PIR`)
- **Signal pin:** `D4` (`kPirPin`)
- Most PIR modules output a **digital gate** (HIGH/LOW).
- Many PIRs are **3.3 V only**—check the label before powering.

**Sketch wiring:**

- PIR OUT → `D4`
- PIR VCC → 3.3 V (or 5 V if explicitly supported)
- PIR GND → board GND

## Recommended RC values

- **No RC needed.** The PIR board has its own analog front‑end.
- If the digital gate chatters, add a **10 kΩ + 100 nF** RC on the output, or just increase the software guard time.

## Calibration steps (live, out loud)

1. **Warm‑up ritual:** wait 10–30 seconds; the code holds output at 0 until stable.
2. **Sensitivity trim:** adjust the PIR’s onboard pot until a slow walk triggers but idle does not.
3. **Guard window:** if it retriggers too fast, increase `guard_us_` in `pir_sensor.cpp`.

## Expected gesture behavior

- **Bow:** broad, slow arm sweeps become a smooth envelope.
- **Pluck:** not ideal—PIR doesn’t do fine taps.
- **Scrape:** use it for “stage presence” rather than micro‑gesture.

## Common failure modes

- **Always HIGH:** wrong voltage or sensor aimed at a hot vent/window.
- **Always LOW:** too short a warm‑up or sensitivity set too low.
- **False triggers:** moving heat sources (sunbeams, heaters) or flimsy mounts.
