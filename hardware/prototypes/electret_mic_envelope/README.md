# Prototype: Electret Mic Envelope

- **Intent:** turn room sound into a playable envelope—whispers, foot taps, or a shared chant can bow the “string.”
- **Parts:** electret mic + preamp (MAX4466, MAX9814, etc.) or a breakout that outputs an analog envelope.
- **Wiring (firmware default):** preamp OUT → **A4**, plus 3.3 V and GND. Keep the cable short to avoid picking up the projector’s EMI.
- **Firmware flag:** compile with `-D SENSOR_ELECTRET`.
- **Calibration ritual:**
  1. Speak softly and watch the envelope rise; if it sticks near zero, raise `gain_` in `firmware/src/electret_mic_sensor.cpp`.
  2. If the room hums, slow the bias follower (`bias_ = 0.9994f * bias_ + 0.0006f * x`) so it ignores AC noise.
  3. Use `on_thresh` / `off_thresh` to separate “audible gesture” from background chatter.
- **Failure modes / compost pile:** clipping makes everything look like a pluck. If the signal slams to 1.0 constantly, reduce gain or add a resistor divider.

