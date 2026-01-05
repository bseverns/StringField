# Prototype: PIR Motion Gate

- **Intent:** detect “presence” and ghost‑bowing. Great for large, slow, theatrical gestures.
- **Parts:** PIR module (HC‑SR501, AM312, etc.) + 3.3 V/5 V supply (check your board).
- **Wiring (firmware default):** PIR digital OUT → **pin 4**, plus VCC + GND. The firmware smooths the on/off gate into a soft envelope.
- **Firmware flag:** compile with `-D SENSOR_PIR`.
- **Calibration ritual:**
  1. **Warm‑up:** give the module 10–30 seconds. The firmware keeps output calm during this window (`warmup_ms_`).
  2. Walk past the sensor and note how long the envelope lingers. Adjust the decay (`env_ = 0.92f * env_ + 0.08f * x`) if you want faster or slower fades.
  3. If the module chatters, increase the guard (`guard_us_`) in `firmware/src/pir_sensor.cpp`.
- **Failure modes / compost pile:** HVAC drafts or sunlight can trigger false positives; PIRs don’t detect stillness, so subtle finger vibrato won’t show up.

