# Prototype: Piezo Contact Mic

- **Intent:** turn taps and scrapes into clear transient energy. This is the “percussive string” variant.
- **Parts:** piezo disc, **1 MΩ** bias resistor, optional diode clamp (1N4148 pair), optional RC envelope.
- **Wiring (firmware default):** piezo → bias resistor to mid‑rail → **A3**, plus GND. The onboard LED (pin 13) blinks on peaks so students can “see” hits.
- **Firmware flag:** compile with `-D SENSOR_PIEZO`.
- **Calibration ritual:**
  1. Tap lightly and watch the LED flicker; if it never lights, raise the `gain` multiplier in `firmware/src/piezo_sensor.cpp`.
  2. If the room hums, slow the bias follower (`bias = 0.999f * bias + 0.001f * x`) so it doesn’t chase noise.
  3. Pair with `on_thresh` / `off_thresh` to make taps count as plucks without triggering on hand tremor.
- **Failure modes / compost pile:** piezos can spike above 3.3 V, so use a clamp if you see weird resets. Cable movement can look like hits—strain‑relief the lead.

