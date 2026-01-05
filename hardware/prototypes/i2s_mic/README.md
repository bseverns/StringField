# Prototype: I²S/PDM Digital Mic

- **Intent:** higher‑fidelity room listening with a clean digital path. Good for workshops where analog noise floors are wild.
- **Parts:** I²S mic breakout (SPH0645, ICS‑43434, INMP441, etc.). Some are PDM; check your board support.
- **Wiring (firmware default):** connect **BCLK**, **LRCLK/WS**, **DOUT**, plus 3.3 V + GND to your MCU’s I²S pins. Pin mapping is board‑specific; follow your Teensy/ESP32 pinout.
- **Firmware flag:** compile with `-D SENSOR_I2S_MIC`.
- **Calibration ritual:**
  1. Confirm `I2S.begin()` succeeds; if not, the firmware will output zeros. Fix wiring before tuning thresholds.
  2. Clap and watch the envelope. If it feels too quiet, raise `gain_` in `firmware/src/i2s_mic_sensor.cpp`.
  3. If the envelope is too sluggish, shorten `window_samples_` to make transients pop.
- **Failure modes / compost pile:** wrong pin mapping is the #1 bug. Also, some boards need explicit I²S pin configuration (platform‑specific) before `I2S.begin()` will work.

