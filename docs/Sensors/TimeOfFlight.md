# Time‑of‑Flight (ToF) Proximity Sensor

Use this when you want **distance** as a gestural axis: bow speed from range change, vibrato from micro‑wobbles, “hover pluck” from a quick dip in range.

## Wiring notes

- **Firmware path:** `firmware/src/time_of_flight_sensor.cpp` (`SENSOR_TOF`)
- **Demo pin:** `A2` expects an **analog envelope** for the in‑class build.
- **Real ToF parts:** VL53L0X / VL53L1X / TMF8801 are I²C. If you move to I²C, keep the output normalized to 0..1 so the gesture code stays unchanged.

**Sketch wiring (analog helper board):**

- ToF analog out → `A2`
- ToF VCC → 3.3 V (most are 2.8–3.3 V)
- ToF GND → board GND

## Recommended RC values

- **If using an analog helper:** start with **10 kΩ + 1 µF** (≈10 ms) for a gentle envelope.
- **If using I²C directly:** no RC needed; do filtering in code.

## Calibration steps (live, out loud)

1. **Set a “home” distance:** hold a hand at ~15 cm and watch the normalized value.
2. **Scale:** adjust the raw‑to‑normalized mapping in `time_of_flight_sensor.cpp` (or the helper board gain) until that pose lands at ~0.5.
3. **Floor clamp:** if idle never returns to 0.0, lift the clamp threshold slightly.

## Expected gesture behavior

- **Bow:** slow in/out motion yields a long arc in the envelope.
- **Pluck:** quick dive toward the sensor creates a sharp peak.
- **Vibrato:** tiny wobbles around a fixed distance give a delicate tremble.

## Common failure modes

- **Ambient IR saturation:** sunlight can pin the reading high.
- **Bad angle:** some ToF parts have narrow cones; side‑on hands vanish.
- **Power mismatch:** a 3.3 V sensor on 5 V can latch or die. Don’t.
