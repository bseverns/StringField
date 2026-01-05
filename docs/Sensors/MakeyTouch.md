# MaKey‑style Touch‑to‑Ground Sensor

This is the punk‑rock “clip it and play it” path: a single alligator lead, a person, and a shared ground. It’s the fastest way to get a class **playing** while you narrate signal paths out loud.

## Wiring notes

- **Firmware path:** `firmware/src/makey_sensor.cpp` (`SENSOR_MAKEY`)
- **Signal pin:** `D2` (`kMakeyPin`)
- **Ground:** share ground between the board and whatever the player is touching.
- Enable `INPUT_PULLUP` so the pin idles **HIGH** until touched.

**Sketch wiring:**

- Board `D2` → MaKey (or foil pad) signal lead.
- Board `GND` → player ground (wrist strap, foil ground plane, or another alligator).

## Recommended RC values

- **No RC needed** for the baseline demo. The firmware handles debouncing with an integrator.
- If your wire run is huge or you’re in a noisy room, add a **100 kΩ** series resistor and **10–100 nF** to ground near the input as a hardware low‑pass.

## Calibration steps (live, out loud)

1. **Confirm idle = HIGH:** open serial plotter; idle should sit near `0.0` after normalization.
2. **Tap cadence:** touch the pad; the envelope should rise to `~1.0` and fall without chatter.
3. **Debounce tuning:** if it flickers on jittery hands, slow the integrator in `makey_sensor.cpp`.

## Expected gesture behavior

- **Pluck:** clean spikes from tap‑release.
- **Bow:** sustained touch reads as a flat plateau (great for “long bow” metaphors).
- **Scrape:** quick repeated taps become rhythmic chatter—use that as a teachable sonic texture.

## Common failure modes

- **No shared ground:** always HIGH, never triggers. Ground is the instrument.
- **Long wires:** antenna behavior → phantom touches. Twist or shorten leads.
- **Static zap:** will look like a sudden pluck. Teach “touch the ground plane first.”
