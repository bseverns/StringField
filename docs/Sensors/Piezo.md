# Piezo / Contact Mic Sensor

This path is pure impact energy: taps, knocks, and scrape transients. It’s a **percussive string** when you want hits to be obvious and dramatic.

## Wiring notes

- **Firmware path:** `firmware/src/piezo_sensor.cpp` (`SENSOR_PIEZO`)
- **Signal pin:** `A3`
- **Bias:** a megaohm resistor to mid‑rail keeps the ADC sane.
- **Clamp:** diodes (or a small TVS) keep the piezo from spiking the input.

**Sketch wiring:**

- Piezo disk lead → `A3` through a **1 MΩ** resistor to 3.3 V (or 5 V if your ADC allows).
- Piezo disk other lead → GND.
- Optional: two **1N4148** diodes to 3.3 V and GND to clamp the swing.

## Recommended RC values

- **Bias resistor:** **1 MΩ** (470 kΩ–2.2 MΩ range is fine).
- **Envelope cap:** **10–47 nF** from `A3` to GND for a bit of smoothing.
- If you need slower decay (longer “bow”), try **100 nF**.

## Calibration steps (live, out loud)

1. **Idle bias check:** with no touch, the normalized value should sit around 0.0.
2. **Tap test:** a finger tap should spike near 1.0 and decay quickly.
3. **Gain control:** tune `swing` scaling in `piezo_sensor.cpp` until quiet taps are readable but don’t clip everything.

## Expected gesture behavior

- **Pluck:** crisp, high peaks—great for sharp string attacks.
- **Scrape:** repeated small spikes create a rough, noisy texture.
- **Bow:** harder to get a smooth sustain unless you add RC smoothing or mount in a resonant surface.

## Common failure modes

- **Floating input:** no bias resistor = chaos noise.
- **Over‑clipping:** missing clamps can slam the ADC rails.
- **Dead disk:** cracked piezo = silence. They’re fragile; mount with love.
