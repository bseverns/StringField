# 90‑Minute Onboarding Bootcamp (StringField Edition)

_A brisk, teachable ramp for newcomers: half studio notebook, half workshop script. Use it solo or read it aloud while a room builds the instrument together._

## The promise

In 90 minutes you’ll:

- **Play** a gesture (pluck/bow/scrape) and see it become motion on‑screen.
- **Understand** how sensors become a normalized signal (0..1).
- **Name** the gesture heuristics in human language, not math‑speak.
- **Adjust** at least one calibration knob on purpose.

If you finish this, you can _teach_ StringField, not just run it.

---

## 0. Pre‑flight (5 minutes)

**You need:**

- A StringField sensor stack + Teensy/ESP32 (or whatever hardware you’ve got).
- USB cable.
- PlatformIO + Processing installed.

**Open these files now:**

- `docs/FirstSession.md` (fast path ritual).
- `firmware/src/gesture_engine.cpp` (gesture logic).
- `firmware/include/gesture_engine.h` (tunable thresholds).
- `software/p5js/README.md` or `software/processing/StringFieldViz.pde` (your visualizer).

---

## 1. The 10‑minute “it works” ritual (10 minutes)

Follow `docs/FirstSession.md` end‑to‑end. Don’t skip steps. The pipeline is the instrument:

**hardware → firmware → serial → bridge → visualizer**

If you see the `PLUCK` state in the visualizer ticker, you are live.

---

## 2. Sensor signal = normalized story (15 minutes)

Open the sensor implementation you’re using:

- `firmware/src/optical_sensor.cpp`
- `firmware/src/capacitive_sensor.cpp`
- `firmware/src/makey_sensor.cpp`
- `firmware/src/time_of_flight_sensor.cpp`
- `firmware/src/piezo_sensor.cpp`
- `firmware/src/pir_sensor.cpp`
- `firmware/src/electret_mic_sensor.cpp`
- `firmware/src/i2s_mic_sensor.cpp`

**Narrate this out loud while you read:**

1. **What raw signal comes in?** (ADC, digital gate, PCM samples)
2. **How do we normalize it?** (map into 0..1)
3. **How do we smooth it?** (low‑pass, envelope, debounce)
4. **What breaks it?** (grounding, ambient light, sensor saturation)

Bonus punk‑rock move: open `docs/Sensors/README.md` and read the failure modes like a checklist. Tech is a performance—own the glitches.

---

## 3. Gesture vocabulary anatomy (15 minutes)

Open `docs/GestureVocabulary.md`. This is your teaching script.

Key files to cross‑reference:

- `firmware/include/gesture_engine.h` (the tuning knobs)
- `firmware/src/gesture_engine.cpp` (the rules)

**Do one live tweak** (even if it’s small):

- Raise `harmonic_hold_us` to make harmonics harder.
- Lower `tremolo_min_delta` to make tremolo easier.
- Tighten `scrape_window_us` to cut accidental scrapes.

Reflash, replay, and narrate what changed. That’s the whole pedagogy loop.

---

## 4. Visualize the signal (15 minutes)

Pick one:

- **p5.js debugger:** `software/p5js/sketch.js`  
  (Teach “data in → motion out” with big labels and velocity bars.)
- **Processing viz:** `software/processing/StringFieldViz.pde`  
  (Project it; the room needs to see the signal breathe.)

**Prompt:** “What does a ‘bow’ _look_ like on screen?”  
Let the room answer before you explain it.

---

## 5. Hardware‑aware tuning (15 minutes)

Choose the sensor page that matches your build and follow the calibration ritual:

- `docs/Sensors/OpticalReflective.md`
- `docs/Sensors/CapacitiveSingleWire.md`
- `docs/Sensors/MakeyTouch.md`
- `docs/Sensors/TimeOfFlight.md`
- `docs/Sensors/Piezo.md`
- `docs/Sensors/PIR.md`
- `docs/Sensors/ElectretMic.md`
- `docs/Sensors/I2SMic.md`

**Goal:** adjust one knob in firmware that maps to a physical sensation.  
Example: “Raise `on_thresh` until the bow only activates when you _mean_ it.”

---

## 6. Exit ticket (15 minutes)

By the end, make sure you can answer (or teach) these questions:

- What does 0..1 mean for your current sensor?
- What separates a pluck from a scrape in code?
- How does the gesture engine avoid false retriggers?
- Which knob would you tune first in a noisy room?

If you can narrate this without staring at the code, you win.

---

## Optional homework (30–60 minutes)

Pick one:

- Run the gesture tests: `pio test -d firmware -e native`
- Add a new sensor note page in `docs/Sensors/`
- Prototype a new “gesture” (e.g., glissando) by sketching rules in `docs/GestureVocabulary.md`

If you write it down, it’s teachable. If it’s teachable, it’s real.
