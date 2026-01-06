# Gesture Vocabulary (Narratable Heuristics)

_This is the “read it out loud” guide to the gesture engine. It explains **why** each gesture exists, **how** we detect it, and **which knobs** to tune when reality gets noisy. Pair this with `firmware/src/gesture_engine.cpp` and the parameter table in `firmware/include/gesture_engine.h`._

## Big idea: gestures are stories, not equations

We don’t look for “perfect signals.” We look for **behavior** that players can repeat and instructors can narrate. Every gesture is a small, human‑readable rule with a few thresholds you can tweak while the room listens.

---

## Gesture quick map (what the engine calls it)

| Gesture | How it feels | What the engine looks for |
| --- | --- | --- |
| **Pluck** | A single, intentional attack | Contact crosses `on_thresh` after enough time since the last onset |
| **Scrape** | Rapid, grainy micro‑attacks | Two onsets closer than `scrape_window_us` |
| **Bow** | Sustained touch / continuous motion | Continuous contact, no special harmonic/tremolo trigger |
| **Harmonic** | Light, steady touch with “glass” tone | Peak stays within `harmonic_peak_min`..`max` **and** stays still for `harmonic_hold_us` |
| **Muted** | Short, damped touch | Release within `mute_window_us` **or** drop below `mute_release_thresh` |
| **Tremolo** | Shallow, fast oscillation | Wobble count hits `wobble_goal` with low depth |
| **Vibrato** | Deeper oscillation | Wobble count hits `wobble_goal` with depth ≥ `vibrato_depth_min` |

---

## The narratable rules (human language first)

### 1) Pluck

**Say this:**  
“A pluck is a clear crossing into contact. If it happens too soon after the last one, we ignore it. If it’s close enough to a prior onset, we call it a scrape instead.”

**Knobs that matter:**  
`on_thresh`, `off_thresh`, `min_retrigger_us`, `scrape_window_us`

---

### 2) Scrape

**Say this:**  
“A scrape is just a **rapid series of onsets**. The time between them is shorter than a pluck.”

**Knobs that matter:**  
`scrape_window_us` (shorter = stricter, longer = more scrapes)

---

### 3) Bow

**Say this:**  
“Bow is the default: it’s what we call continuous contact when nothing else is true. It’s sustained energy with no special wobble or harmonic signature.”

**Knobs that matter:**  
Mostly `on_thresh` and `off_thresh`, plus smoothing in your sensor front‑end.

---

### 4) Harmonic

**Say this:**  
“A harmonic is a **light, steady touch**. You hit a gentle peak, then hold it still long enough to earn the glassy tone.”

**Knobs that matter:**  
`harmonic_peak_min`, `harmonic_peak_max`, `harmonic_hold_us`, `harmonic_variation_eps`

**Calibration ritual:**

1. Lightly graze the sensor.
2. Nudge `harmonic_peak_min`/`max` until harmonics trigger without full plucks.
3. Tighten `harmonic_variation_eps` until the system demands stillness.

---

### 5) Muted

**Say this:**  
“A mute is a **short, damped touch**. If the contact ends quickly, or the signal drops below the mute floor, we call it muted.”

**Knobs that matter:**  
`mute_peak_thresh`, `mute_window_us`, `mute_release_thresh`

**Teaching cue:**  
Ask the class to “tap and vanish” — quick contact then immediate release.

---

### 6) Tremolo

**Say this:**  
“Tremolo is a **fast, shallow wobble**. The engine counts sign flips and calls it once it sees enough.”

**Knobs that matter:**  
`tremolo_min_delta`, `tremolo_max_period_us`, `tremolo_grace_us`, `wobble_goal`

---

### 7) Vibrato

**Say this:**  
“Vibrato is tremolo with more depth. Same wobble count, bigger swing.”

**Knobs that matter:**  
`vibrato_depth_min` (raise it to demand bigger swings)

---

## Threshold cheat‑sheet (firmware knobs)

These live in `firmware/include/gesture_engine.h`. Keep these names in your teaching script so students can go from “feel” to “code” without translation.

- **Contact gates:** `on_thresh`, `off_thresh`
- **Time guards:** `min_retrigger_us`, `scrape_window_us`, `harmonic_hold_us`, `mute_window_us`, `tremolo_grace_us`
- **Harmonic band:** `harmonic_peak_min`, `harmonic_peak_max`, `harmonic_variation_eps`
- **Mute window:** `mute_peak_thresh`, `mute_release_thresh`
- **Wobble rules:** `tremolo_min_delta`, `tremolo_max_period_us`, `wobble_goal`, `vibrato_depth_min`

---

## In‑class debugging prompts (keep it human)

- “Is the sensor normalized to 0..1?” (Check your sensor front‑end.)
- “Do we see the peak hit the harmonic band?” (Plot the envelope.)
- “Is the wobble count too strict?” (Lower `wobble_goal` or `tremolo_min_delta`.)
- “Are we double‑triggering?” (Raise `min_retrigger_us`.)

---

## Next expansion ideas (for brave collaborators)

- **Glissando**: detect steady contact with a monotonic rise/fall in value.
- **Ghost note**: sub‑threshold taps that light the visualizer but don’t send MIDI.
- **Scrape texture index**: count scrape grains per second and map it to CC.

If you prototype a new gesture, describe it here first. Code comes second.
