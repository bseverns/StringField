# I²S / PDM Digital Mic

This is the **cleanest audio path**: a digital mic that spits out PCM. It’s less fragile than electrets and gives you repeatable envelopes for gestures.

## Wiring notes

- **Firmware path:** `firmware/src/i2s_mic_sensor.cpp` (`SENSOR_I2S_MIC`)
- **Interfaces:** I²S (WS/LRCLK, BCLK, DATA) with power/ground.
- Common parts: **SPH0645**, **ICS‑43434**, **INMP441**.

**Sketch wiring (generic):**

- Mic `WS/LRCLK` → board I²S WS pin
- Mic `BCLK` → board I²S BCLK pin
- Mic `DATA` → board I²S data in
- Mic `VDD` → 3.3 V
- Mic `GND` → GND

(Exact pin mapping depends on your board; follow the board’s I²S pinout.)

## Recommended RC values

- **No RC needed.** This is digital; all shaping happens in firmware.

## Calibration steps (live, out loud)

1. **Confirm I²S starts:** if `ready_` stays false, check wiring and clock pins.
2. **Gain tuning:** adjust `gain_` in `i2s_mic_sensor.cpp` so quiet taps are visible.
3. **Window size:** tweak `window_samples_` for faster or smoother envelopes.

## Expected gesture behavior

- **Pluck:** sharp peaks from taps or snaps.
- **Scrape:** textured noise bands; works well for “scratch bow” metaphors.
- **Bow:** smoother swells if you lengthen the envelope window.

## Common failure modes

- **No clock lock:** wrong WS/BCLK pins = total silence.
- **Word‑select swapped:** sounds like garbage noise (values jump wildly).
- **Sample‑rate mismatch:** aliasy hiss or sluggish response.
