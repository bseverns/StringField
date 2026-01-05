# StringField Build Kit + Educator Packet

_A one-stop, punk-adjacent build binder for instructors who need to go from box-of-parts to first-play in a single class block._

## Bill of Materials (final-ish, price in USD)

| Part | Suggested part # | Qty | Notes | Cost range |
| --- | --- | --- | --- | --- |
| Microcontroller | **Teensy 4.0** (PJRC) | 1 | Default firmware target; USB MIDI class-compliant. | $22–$30 |
| Alt MCU | **ESP32-S3-DevKitC-1-N8** (Espressif) | 1 | Wireless demos; compile with `env:esp32s3`. | $12–$18 |
| Power | USB‑C cable + 5V/1A wall brick | 1 | Keep it isolated from noisy laptop ports when demoing. | $8–$15 |
| Optical sensor | **TCRT5000 reflective IR pair** or **QRE1113GR** | 1 | Mount 10–25 mm from hand path. | $0.50–$2 |
| Capacitive string | 26–30 AWG insulated hookup wire | 1–2 m | Suspend it; avoid kinks. | $1–$3 |
| MaKey-style touch | Foil tape strip + alligator clip | 1 | Acts as string; route to GPIO with pull-up. | $2–$4 |
| Proximity | **VL53L0X** or **TMF8801** breakout | 1 | Tie to `A2` (analog envelope) or I²C if you swap in the full driver later. | $6–$15 |
| Impact | Piezo disc (27 mm) + 1 MΩ resistor | 1 | Bias then clamp extremes on `A3`. | $1–$3 |
| Audio/MIDI jack | TRS‑MIDI breakout **(SparkFun BOB‑14450)** | 1 | Optional 5‑pin/TRS MIDI out. | $6–$10 |
| LED indicator | 5 mm diffused LED + 220 Ω resistor | 1 | Blink for “hit seen” demos. | $0.50–$1 |
| Breadboard | 400–830 tie-point solderless | 1 | Keep student rewires low-friction. | $4–$8 |
| Wiring | Assorted male/female jumpers | 20+ | Color-code per sensor stack. | $3–$6 |

> Local sourcing friendly: swap Teensy 4.0 for any 3.3 V MCU with 4+ analog inputs; keep thresholds editable in `config.h` and scale JSON interface untouched.

## Wiring snapshots (per sensor stack)

ASCII is printable. Pin numbers assume Teensy 4.0; ESP32-S3 just map the labels to analog-capable pins.

### Optical (reflective IR)

```
[3.3V] ──(100Ω-470Ω)──▶ IR LED anode
GND ───────────────────▶ IR LED cathode & phototransistor emitter
Phototransistor collector ──(10k pull-up to 3.3V)──▶ A1
Optional: LED drive PWM on pin 5 if you want modulation.
```

Mount the pair ~15 mm from the hand path, shroud from room light.

### Capacitive single-wire

```
Suspended insulated wire “string” ──▶ A0 (high‑Z)
Player ground clip ─────────────────▶ GND
Optional: driven guard ring/shield tied to A0 guard if your board supports it.
```

Keep laptop earth loops away; tape down the ground clip to avoid surprise detaches.

### MaKey-style touch-to-ground

```
Foil/string “string” ──▶ GPIO 12 (INPUT_PULLUP)
Player ground clip ───▶ GND
Optional: 1 nF–10 nF to ground at GPIO to tame chatter.
```

Treat like a digital button; we still low-pass in firmware to narrate “press” vs. “brush.”

### Time-of-Flight proximity

```
VL53L0X/TMF8801 breakout VCC ─▶ 3.3V
Breakout GND ────────────────▶ GND
Breakout SDA/SCL ────────────▶ 18 / 19 (I²C)
Optional analog envelope out ─▶ A2 (for quick-and-dirty demos with `-D SENSOR_TOF`).
```

Clamp the sensor face in a 3D-printed hood to block side spill; keep ribbon cables short.

### Piezo contact mic

```
Piezo + ───────────────▶ A3 (through 1 MΩ bias to 1.65 V mid-rail)
Piezo – ───────────────▶ GND
LED indicator (pin 13) ─▶ 220 Ω ─▶ LED ─▶ GND (optional “hit seen” lamp)
```

Tape the disc to the playing surface edge; add foam for isolation if the table booms.

## Flashing the firmware (Teensy and ESP32-S3)

1. Install [PlatformIO Core](https://platformio.org/install) or use the VS Code extension.
2. Plug in the board via USB; select the right port (`/dev/ttyACM*` for Teensy, `/dev/ttyUSB*` for ESP32-S3).
3. From the repo root, build + upload:
   - Teensy 4.0: `pio run -e teensy40 -t upload`
   - ESP32-S3 DevKitC: `pio run -e esp32s3 -t upload`
4. Open a serial monitor at **115200 baud** (`pio device monitor` or your favorite terminal).
5. Throw a quick gesture to see the boot telemetry; paste `{"notes":[60,63,67,70,74]}` to hot-swap the scale and prove runtime control works.

## Calibration micro-rituals (two minutes per class)

Straight from the README, because repeatability beats mystique:

1. **Set the contact floor/ceiling** using `on_thresh` / `off_thresh` while watching the Serial feed. Avoid chatter.
2. **Sweep the light touch band**: lightly graze the sensor and nudge `harmonic_peak_min`/`max` and `harmonic_variation_eps` until harmonics trigger without full plucks.
3. **Mute window sanity**: tap and lift quickly; adjust `mute_window_us` and `mute_release_thresh` if students have slow reflexes or the room hums.
4. **Wobble feel**: shake your finger above the sensor; set `tremolo_min_delta`, `tremolo_grace_us`, and `wobble_goal` so tremolo happens at intentional oscillations, not random noise.

## Printable handouts (two-sided cheat cards)

Print 2-up on letter paper, double-sided:

**Side A: Build/Wire Map**

- BOM mini-table with pin map per sensor (copy the wiring snippets above).
- Serial hotkeys: `115200` baud, paste JSON scales, `?` to echo config (firmware prints help on boot).
- Safety mantra: "low voltage, clear ground clip, stop if something gets warm."

**Side B: Calibration + Consent**

- The four micro-rituals above with checkbox squares.
- Blank lines for today’s thresholds (`on/off_thresh`, `harmonic_peak_min/max`, `mute_window_us`, `tremolo_min_delta`).
- Consent reminder: "No recording unless everyone agrees; offline-first."

## First-play lesson plan (45–60 minutes)

1. **Hook + safety (5 min):** Show the sensor string, hand everyone a ground clip; declare low-voltage, no recordings.
2. **Assemble fast (10 min):** Split into squads: optical stack, capacitive stack, piezo stack. Each squad follows the wiring snapshot and initial thresholds.
3. **Flash + ping (5 min):** One laptop per squad runs the matching `pio run -t upload` command; everyone sends the JSON scale blob to prove the loop.
4. **Calibrate live (10 min):** Rotate through the four micro-rituals. Let students narrate what changed on Serial so the class hears the mapping logic.
5. **Gesture tour (10 min):** Demo pluck, bow-ish scrape, harmonic touch, mute, and wobble → MIDI CC/pitch bend. Use the Processing/p5.js visualizer if you have a projector.
6. **Swap sensors (5–10 min):** Trade stacks between squads without changing the gesture mapping; reflect on what stayed stable (interface) vs. what shifted (thresholds).
7. **Exit ticket (5 min):** Each student writes one mapping tweak they’d ship next class and snaps the calibrated thresholds into the handout.

## Optional sensor squads (advanced / week‑two build)

If your crew wants a bigger palette, these are ready in firmware and documented in `hardware/prototypes/`:

- **Time‑of‑flight proximity:** `hardware/prototypes/time_of_flight_proximity/README.md` (compile with `-D SENSOR_TOF`).
- **PIR motion gate:** `hardware/prototypes/pir_motion_gate/README.md` (compile with `-D SENSOR_PIR`).
- **Electret mic envelope:** `hardware/prototypes/electret_mic_envelope/README.md` (compile with `-D SENSOR_ELECTRET`).
- **I²S/PDM mic:** `hardware/prototypes/i2s_mic/README.md` (compile with `-D SENSOR_I2S_MIC`).

These are great for second‑session experiments: swap them in without changing mappings, then compare how each sensor shapes the same gesture vocabulary.

## Facilitator notes

- Keep jumpers color-coded per sensor (optical = purple/black, capacitive = white/green, piezo = yellow) so classroom chaos stays legible.
- If a student asks “why this part number?”, point at the cost ranges and offer an alternate; remixing parts is part of the lesson.
- Your job is narration, not wizardry. Say the thresholds out loud, show the Serial scroll, and let them touch the knobs.
