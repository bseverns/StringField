# Prototype: Time‑of‑Flight Proximity (ToF)

- **Intent:** a hands‑in‑air “string” that reads distance instead of touch. Great for open‑air gestures and big, theatrical bows.
- **Parts:** a ToF breakout (VL53L0X, VL53L1X, TMF8801, etc.) or a helper board that exposes an analog envelope.
- **Wiring (firmware default):** wire the analog envelope to **A2** + GND. The current firmware expects an analog 0–3.3 V signal so the class can swap boards without touching the gesture code.
- **Firmware flag:** compile with `-D SENSOR_TOF`.
- **Calibration ritual:**
  1. Hold a hand at the “string” distance you want as **neutral**, then watch the Serial plotter and adjust `on_thresh` / `off_thresh`.
  2. If the envelope feels laggy, open `firmware/src/time_of_flight_sensor.cpp` and tweak the smoothing blend (`0.85/0.15`).
  3. If your room never hits true zero, bump the floor clamp (currently `0.02f`) until idle reads ~0.0.
- **Failure modes / compost pile:** shiny surfaces can scatter IR; fluorescent lights can pollute the readings; I²C ToF boards may need level shifting if you later switch to a full driver.

