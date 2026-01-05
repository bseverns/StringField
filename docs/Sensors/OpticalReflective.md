# Optical Reflective IR (Field Notes)

_Goal: a fast, cheap “string in air” that’s readable in class. This page is meant
to be read out loud while you wire._

## What it feels like

Reflective IR is **bow‑friendly** and **scrape‑friendly**. It catches continuous
motion better than micro‑taps, and it’s sensitive to ambient light, surface
albedo, and room layout. Use it when you want the “string” to be visible and
performative.

---

## Wiring sketch (prototype‑level)

- **Emitter (IR LED):** 5V → resistor → IR LED → GND  
  (pick a resistor that keeps the LED cool: start with 100–220Ω)
- **Receiver (phototransistor):** collector → 5V, emitter → resistor → GND  
  tap the emitter (or resistor midpoint) into `A0`

**Expected signal range:** analog 0..1023 (ADC).  
**Firmware file:** `firmware/src/optical_sensor.cpp`

---

## Calibration ritual (3 minutes)

1. **Dark baseline:** cover the sensor, watch the serial plotter. That’s your
   “quiet” floor.
2. **Hand at distance:** hover at your intended playing distance. You should see
   the envelope rise.
3. **Thresholds:** raise `on_thresh` until light chatter stops, then lower it
   just enough to catch intentional contact.

---

## Narratable failure modes (aka “what’s wrong” in class)

- **Always high:** ambient light or a reflective surface is flooding the sensor.
  Shield it, angle it down, or move away from windows.
- **Always low:** emitter LED wired backwards or dead. Check polarity and
  resistor.
- **Noisy flicker:** fluorescent lighting or PWM LEDs nearby; increase smoothing
  or add a light shield.

---

## Teach the normalization

Explain it like this:

> “We read a raw voltage, squeeze it into 0..1, subtract a moving ambient
> baseline, and smooth it so the gesture engine sees _motion energy_ instead of
> raw flicker.”

That one sentence turns a blinking LED into a teachable instrument.
