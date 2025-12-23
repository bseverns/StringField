// StringField firmware (v0.1)
// Target: Teensy 4.0 by default. Optional: ESP32-S3.
// -----------------------------------------------------------------------------
// INTENT:
//  - Provide a minimal, well-commented path from sensor reading → gesture naming
//    (pluck/bow/scrape) → MIDI events. Keep each layer swappable.
//  - Make trade-offs explicit: thresholds, guards, smoothing, timing windows.
//  - Prefer clarity over cleverness; this is a teaching instrument.
// -----------------------------------------------------------------------------

#include <Arduino.h>
#include <MIDI.h>
#if defined(TEENSYDUINO)
#include <usb_serial.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gesture_engine.h"

// ---- Compile-time selection of sensing path ---------------------------------
// Define exactly one of these in platformio.ini build_flags, e.g. -D SENSOR_OPTICAL
#if !defined(SENSOR_OPTICAL) && !defined(SENSOR_CAPACITIVE) && !defined(SENSOR_MAKEY) && !defined(SENSOR_TOF) && !defined(SENSOR_PIEZO) && !defined(SENSOR_PIR) && !defined(SENSOR_ELECTRET) && !defined(SENSOR_I2S_MIC)
  #define SENSOR_OPTICAL 1  // default demo; explicitly include new ToF/Piezo/PIR/Mic options above
#endif

// ---- MIDI setup --------------------------------------------------------------
#if defined(TEENSYDUINO)
// Teensy exposes `Serial` as a USB CDC endpoint (usb_serial_class), not a
// HardwareSerial port. The default MIDI macro assumes a UART, so we override the
// transport here to bind the 47effects MIDI stack directly to the USB link the
// classroom laptops actually plug into.
MIDI_CREATE_INSTANCE(usb_serial_class, Serial, MIDI);
#else
MIDI_CREATE_DEFAULT_INSTANCE();
#endif
static const uint8_t kChannel = 1;  // 1-16

// Abstract sensor interface so gesture code doesn't care which hardware we use.
/**
 * Abstract sensing interface. The firmware intentionally hides the details of
 * which physical stack is active so students can swap implementations without
 * rewriting the gesture logic. All derived classes *must* implement `begin`
 * (to configure pins/ICs) and `read` (to produce a normalized SensorSample).
 */
class Sensor {
 public:
  virtual void begin() = 0;
  virtual SensorSample read() = 0;
  virtual ~Sensor() {}
};

// ---- Optical demo sensor (placeholder) --------------------------------------
#ifdef SENSOR_OPTICAL
class OpticalSensor : public Sensor {
 public:
  void begin() override {
    // INTENT: keep pin assignments centralized; swapping pins should not touch logic.
    pinMode(A0, INPUT);    // phototransistor on A0 (via resistor)
    pinMode(13, OUTPUT);   // onboard LED for heartbeat
  }
  SensorSample read() override {
    int raw = analogRead(A0);  // 0..1023 on Teensy (will be 12-bit on some MCUs)
    // Map raw to 0..1 with a crude low-pass to tame flicker; tune in calibration.
    static float y = 0.0f;
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    y = 0.9f * y + 0.1f * x;
    digitalWrite(13, (millis() >> 6) & 1);  // slow blink to show life
    return {y, micros()};
  }
};
#endif

// ---- Capacitive / MaKey implementations --------------------------------------
#ifdef SENSOR_CAPACITIVE
/**
 * RC-based capacitive read with built-in calibration and guard timing.
 *
 * Why the ceremony?
 *   - We discharge the pad between reads so the RC ramp starts from 0 V.
 *   - We enforce a guard window so back-to-back reads do not ghost from the
 *     previous charge.
 *   - We continuously re-learn the baseline so humid rooms do not tank the
 *     signal. Treat `settle_us_` and `guard_us_` as knobs to narrate in class.
 */
class CapacitiveSensorCalibrated : public Sensor {
 public:
  void begin() override {
    pinMode(kPadPin, INPUT);
    // Pre-fill the baseline with a quick average so the first loop iteration
    // does not spike. Students can watch this in the serial plotter.
    uint32_t sum = 0;
    for (int i = 0; i < 16; ++i) {
      sum += measure_raw();
      delay(2);
    }
    baseline_ = sum / 16.0f;
  }

  SensorSample read() override {
    uint32_t now = micros();
    // Guard: if a previous read happened too recently, reuse the last sample.
    if (now - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now;

    uint16_t raw = measure_raw();
    // Slow baseline drift follower; this resists humidity swings but keeps
    // quick touches visible. The rate is intentionally tiny for classroom calm.
    baseline_ = 0.999f * baseline_ + 0.001f * raw;
    float delta = raw - baseline_;
    float normalized = constrain(delta / sensitivity_scale_, 0.0f, 1.0f);
    last_sample_ = {normalized, now};
    return last_sample_;
  }

 private:
  static const uint8_t kPadPin = A1;  // pad → A1, driven against GND bracelet
  const uint16_t settle_us_ = 50;     // let the pullup charge the pad before sampling
  const uint16_t discharge_us_ = 200; // drain to ground so each read starts clean
  const uint16_t guard_us_ = 1000;    // 1ms guard between reads to avoid ghosting
  const float sensitivity_scale_ = 400.0f;  // tweak alongside on/off thresholds

  float baseline_ = 0.0f;
  uint32_t last_read_us_ = 0;
  SensorSample last_sample_{0.0f, 0};

  uint16_t measure_raw() {
    // Drain any residual charge. This assumes the performer is grounded via a
    // wrist strap or shared foil so the pad always has a reference.
    pinMode(kPadPin, OUTPUT);
    digitalWrite(kPadPin, LOW);
    delayMicroseconds(discharge_us_);

    // Charge and measure the RC rise time using the built-in ADC. On Teensy
    // this is ~10-bit; if your board has higher resolution, keep the 0..1023
    // normalize above and narrate the change.
    pinMode(kPadPin, INPUT_PULLUP);
    delayMicroseconds(settle_us_);
    return analogRead(kPadPin);
  }
};
#endif

#ifdef SENSOR_MAKEY
/**
 * MaKey-style touch sensor: digital input with debounce + guard.
 *
 * Classroom note: give the player a foil/velcro wrist ground so the pull-up
 * has a solid reference. `guard_us_` keeps accidental double plucks at bay and
 * the simple debounce keeps all narration visible in the serial plotter.
 */
class MakeySensorGuarded : public Sensor {
 public:
  void begin() override {
    pinMode(kMakeyPin, INPUT_PULLUP);
  }

  SensorSample read() override {
    uint32_t now = micros();
    if (now - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now;

    bool touch = digitalRead(kMakeyPin) == LOW;  // MaKey shorts to ground
    // Debounce using a tiny integrator so hand jitter does not look like a bow.
    debounce_state_ = 0.9f * debounce_state_ + 0.1f * (touch ? 1.0f : 0.0f);
    float normalized = debounce_state_;

    last_sample_ = {normalized, now};
    return last_sample_;
  }

 private:
  static const uint8_t kMakeyPin = 2;  // MaKey output wired here (internal pullup enabled)
  const uint16_t guard_us_ = 2000;     // 2ms guard to avoid chatter

  uint32_t last_read_us_ = 0;
  float debounce_state_ = 0.0f;
  SensorSample last_sample_{0.0f, 0};
};
#endif

// ---- Time-of-Flight proximity sketch ----------------------------------------
// This intentionally mirrors the simple optical stub so the same calibration
// flow works: pick a pin, read raw depth, map to 0..1, and narrate every tradeoff.
#ifdef SENSOR_TOF
class TimeOfFlightSensor : public Sensor {
 public:
  void begin() override {
    // Pin/PWR notes: real ToF breakouts (VL53L0X, TMF8801, etc.) use I2C. Here we
    // map the analog envelope from a helper board onto A2 to keep the demo
    // solderable and readable in-class. Swap the pin when moving to a true I2C
    // driver; keep the function shape identical so GestureEngine stays agnostic.
    pinMode(A2, INPUT);
  }
  SensorSample read() override {
    int raw = analogRead(A2);  // e.g., 0..1023. Replace with mm reading / 4096.0 for I2C parts.
    // Calibrate like a lab notebook: expose the bias and smoothing knobs. Students
    // can anchor a “hand at 15 cm” pose and tune the filter and scaling live.
    static float y = 0.0f;
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    y = 0.85f * y + 0.15f * x;  // slightly faster than optical to catch hand waves
    // Cheap floor clamp for noisy rooms; edit in class if your sensor never
    // truly rests at 0.0 while idle.
    if (y < 0.02f) y = 0.0f;
    return {y, micros()};
  }
};
#endif

// ---- Piezo contact microphone sketch ----------------------------------------
// The piezo is the scrappy friend of the lab: it needs a bias resistor and a
// clamp diode. We AC-couple by subtracting the mid-rail estimate so this class
// outputs a unipolar 0..1 envelope the GestureEngine can digest.
#ifdef SENSOR_PIEZO
class PiezoSensor : public Sensor {
 public:
  void begin() override {
    pinMode(A3, INPUT);
    pinMode(13, OUTPUT);  // re-use the onboard LED to show when peaks land
  }
  SensorSample read() override {
    static float bias = 0.5f;  // mid-rail estimate in normalized units
    int raw = analogRead(A3);
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    bias = 0.999f * bias + 0.001f * x;  // slow bias tracker; tweak in calibration session
    float swing = fabs(x - bias) * 2.2f;  // exaggerate small hits; clip later
    float env = constrain(swing, 0.0f, 1.0f);
    digitalWrite(13, env > 0.4f);  // punk-rock peak lamp
    return {env, micros()};
  }
};
#endif

// ---- Passive infrared (PIR) motion sketch ----------------------------------
// PIR modules latch high when they detect warm bodies moving. We deglitch the
// output and add a slow decay so the serial plots show a “motion envelope”
// instead of binary edges. Keep the warm-up and guard intervals narratable.
#ifdef SENSOR_PIR
class PirSensor : public Sensor {
 public:
  void begin() override {
    pinMode(kPirPin, INPUT);  // many PIR boards expose a digital gate; some are 3.3 V only
    warmup_start_ms_ = millis();
  }

  SensorSample read() override {
    uint32_t now_us = micros();
    if (!warmed_up()) {
      // Most PIRs need 10–30 seconds to stabilize; keep the plot calm until then.
      last_sample_ = {0.0f, now_us};
      return last_sample_;
    }

    bool raw_motion = digitalRead(kPirPin) == HIGH;
    float x = raw_motion ? 1.0f : 0.0f;
    // Exponential decay so a single person pass shows as a hill, not a square wave.
    env_ = 0.92f * env_ + 0.08f * x;
    // Guard window to avoid rapid re-triggers from onboard comparators that chatter.
    if (now_us - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now_us;

    last_sample_ = {env_, now_us};
    return last_sample_;
  }

 private:
  static const uint8_t kPirPin = 4;    // swap per build; keep on an interrupt-capable pin if you later move to ISRs
  const uint32_t warmup_ms_ = 30000;   // adjust if your module stabilizes faster/slower
  const uint32_t guard_us_ = 20000;    // 20 ms to smooth comparator chatter

  uint32_t warmup_start_ms_ = 0;
  float env_ = 0.0f;
  uint32_t last_read_us_ = 0;
  SensorSample last_sample_{0.0f, 0};

  bool warmed_up() const { return (millis() - warmup_start_ms_) > warmup_ms_; }
};
#endif

// ---- Electret microphone (analog envelope) ----------------------------------
// An electret capsule + bias resistor into an analog pin gives an AC waveform.
// We track the DC bias, rectify, and expose a smoothed envelope for the
// GestureEngine. This keeps calibration visible: bias speed, gain, and clamp.
#ifdef SENSOR_ELECTRET
class ElectretMicSensor : public Sensor {
 public:
  void begin() override {
    pinMode(kMicPin, INPUT);
    // Pre-charge bias with a tiny average to avoid a jump on first loop.
    float seed = 0.5f;
    for (int i = 0; i < 8; ++i) {
      seed = 0.75f * seed + 0.25f * sample_raw();
      delay(2);
    }
    bias_ = seed;
  }

  SensorSample read() override {
    uint32_t now = micros();
    float x = sample_raw();
    // AC coupling: follow bias slowly, measure swing fast.
    bias_ = 0.9994f * bias_ + 0.0006f * x;  // tweak live if the room hums
    float swing = fabs(x - bias_) * gain_;
    env_ = 0.88f * env_ + 0.12f * constrain(swing, 0.0f, 1.0f);
    // Small floor clamp to avoid whisper-noise. Bump lower if you need pianissimo.
    if (env_ < 0.01f) env_ = 0.0f;
    return {env_, now};
  }

 private:
  static const uint8_t kMicPin = A4;  // analog envelope input; keep wiring short
  const float gain_ = 3.5f;           // adjust alongside bias speed during calibration

  float bias_ = 0.5f;
  float env_ = 0.0f;

  float sample_raw() { return constrain(analogRead(kMicPin) / 1023.0f, 0.0f, 1.0f); }
};
#endif

// ---- I2S microphone (digital PDM/PCM) ---------------------------------------
// Some classrooms already have digital mics (SPH0645, ICS-43434). We read a
// burst of samples from the I2S bus, compute an envelope, and expose the same
// 0..1 intensity the GestureEngine expects. Swap sample rate / bit depth per
// board; the smoothing constants are meant to be twiddled aloud.
#ifdef SENSOR_I2S_MIC
#include <I2S.h>

class I2SMicSensor : public Sensor {
 public:
  void begin() override {
    // Default to 16 kHz mono; adjust for your part. If begin() fails, keep
    // reading zeros so the rest of the firmware stays predictable in class.
    if (!I2S.begin(I2S_PHILIPS_MODE, sample_rate_, bits_)) {
      ready_ = false;
    } else {
      ready_ = true;
    }
  }

  SensorSample read() override {
    uint32_t now = micros();
    if (!ready_) return {0.0f, now};

    int32_t total = 0;
    int16_t sample = 0;
    uint16_t count = 0;
    while (I2S.available() && count < window_samples_) {
      sample = static_cast<int16_t>(I2S.read());
      total += abs(sample);
      ++count;
    }

    float avg = (count > 0) ? (total / static_cast<float>(count)) : 0.0f;
    // Normalize 16-bit PCM to 0..1 and overdrive slightly for quiet rooms.
    float normalized = constrain((avg / 32768.0f) * gain_, 0.0f, 1.0f);
    env_ = 0.9f * env_ + 0.1f * normalized;
    return {env_, now};
  }

 private:
  const int sample_rate_ = 16000;
  const int bits_ = 16;
  const uint16_t window_samples_ = 64;  // ~4 ms at 16 kHz; short for fast articulation
  const float gain_ = 2.5f;

  bool ready_ = false;
  float env_ = 0.0f;
};
#endif

// ---- Mapping -----------------------------------------------------------------
// Map gestures to MIDI: notes from a small pentatonic set, CC1 for bow energy.
static const uint8_t kMaxNoteSlots = 12;

struct NoteSet {
  /**
   * Mapping Narration:
   *   - pluck → NoteOn velocity burst (percussive attack),
   *   - bow   → CC1 envelope stream (continuous breath),
   *   - scrape→ micro-note grains (textural grit).
   * Educators: read this aloud when walking the class through the switch-case below.
   */
  uint8_t notes[kMaxNoteSlots] = {60, 62, 64, 67, 69};  // default: C D E G A
  uint8_t size = 5;
  uint8_t idx = 0;
} g_notes;

/**
 * Round-robin note helper. Any time a pluck or scrape needs another pitch we
 * advance through the active note set. If the set is empty (which can happen if
 * a malformed preset is loaded) we fall back to middle C and reset the counter
 * so the instrument never wedges itself into silence mid-class.
 */
uint8_t next_note() {
  if (g_notes.size == 0) {
    g_notes.notes[0] = 60;
    g_notes.size = 1;
    g_notes.idx = 0;
    return 60;
  }
  uint8_t n = g_notes.notes[g_notes.idx];
  g_notes.idx = (g_notes.idx + 1) % g_notes.size;
  return n;
}

// Track the sustaining note globally so both the gesture mapper and the serial
// hot-swapper can see it. Declaring it before the anonymous namespace keeps the
// linker happy (Teensy's GCC was grumbling when the namespace tried to `extern`
// something defined later in the file) and makes the flow of control easier to
// narrate while screen-sharing.
int current_note = -1;  // -1 == no sustaining note; >=0 stores the active MIDI pitch.

// ---- Serial preset browser ---------------------------------------------------
namespace {
  static char serial_buf[256];
  static size_t serial_len = 0;
  uint8_t last_bow_cc = 0;

  /**
   * Emit a projector-friendly JSON telemetry line. The visualizers depend on
   * the shape `{ "gesture": "pluck", "value": 90, "note": 64 }` so we
   * centralize the formatting and make sure every pathway uses the same voice.
   */
  void emit_gesture_event(const char* name, uint8_t value, int note) {
    Serial.print('{');
    Serial.print("\"gesture\":\"");
    Serial.print(name);
    Serial.print("\"");
    Serial.print(",\"value\":");
    Serial.print(value);
    if (note >= 0) {
      Serial.print(",\"note\":");
      Serial.print(note);
    }
    Serial.println('}');
  }

  /**
   * Tiny, explicit JSON reader for note-set blobs. The firmware only needs the
   * `notes` array and avoids pulling in a full parser. Because classes read the
   * code aloud, everything that could fail is commented in plain language.
   */
  bool parse_note_set_json(const char* line, NoteSet* out) {
    if (!line || !out) return false;
    const char* notes_key = strstr(line, "\"notes\"");
    if (!notes_key) return false;
    const char* open = strchr(notes_key, '[');
    const char* close = open ? strchr(open, ']') : nullptr;
    if (!open || !close || close <= open) return false;

    uint8_t tmp[kMaxNoteSlots] = {0};
    uint8_t count = 0;
    const char* p = open + 1;
    while (p < close && count < kMaxNoteSlots) {
      while (p < close && (*p == ' ' || *p == '\t' || *p == ',')) ++p;
      if (p >= close) break;
      char* endptr = nullptr;
      long val = strtol(p, &endptr, 10);
      if (endptr == p) return false;  // malformed token
      if (val < 0 || val > 127) return false;
      tmp[count++] = static_cast<uint8_t>(val);
      p = endptr;
    }
    if (count == 0) return false;

    memcpy(out->notes, tmp, sizeof(tmp));
    out->size = count;
    out->idx = 0;
    return true;
  }

  /**
   * After successfully loading a scale, echo it back. This acts as the serial
   * UI confirmation and gives the visualizer context for what pitches are in
   * play. The response is still machine-readable for any classroom tooling.
   */
  void acknowledge_noteset(const NoteSet& set) {
    Serial.print('{');
    Serial.print("\"noteset\":\"loaded\",\"count\":");
    Serial.print(set.size);
    Serial.print(",\"notes\":[");
    for (uint8_t i = 0; i < set.size; ++i) {
      Serial.print(set.notes[i]);
      if (i + 1 < set.size) Serial.print(',');
    }
    Serial.println("]}");
  }

  /**
   * Process a single newline-delimited command from the Serial terminal. This
   * is intentionally tiny: either load a new note set, print help, or ignore.
   */
  void handle_serial_line(const char* line) {
    if (!line) return;
    // Minimal punk-rock JSON parser: expects {"notes":[..]}
    NoteSet candidate = g_notes;
    if (parse_note_set_json(line, &candidate)) {
      if (current_note >= 0) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
        emit_gesture_event("release", 0, current_note);
        current_note = -1;
      }
      g_notes = candidate;
      acknowledge_noteset(g_notes);
      return;
    }
    if (strstr(line, "help")) {
      Serial.println("{\"help\":\"Send {\\\"notes\\\":[60,62,...]} to audition scales; this box will echo what it loads.\"}");
    }
  }

  /**
   * Collect characters from Serial until we see a newline, then ship the line
   * to `handle_serial_line`. This keeps the main loop non-blocking and makes it
   * crystal clear to students where serial parsing lives.
   */
  void pump_serial_commands() {
    while (Serial.available() > 0) {
      char c = static_cast<char>(Serial.read());
      if (c == '\r') continue;
      if (c == '\n') {
        serial_buf[serial_len] = '\0';
        if (serial_len > 0) handle_serial_line(serial_buf);
        serial_len = 0;
      } else if (serial_len + 1 < sizeof(serial_buf)) {
        serial_buf[serial_len++] = c;
      } else {
        serial_len = 0;  // overflow: reset buffer, stay resilient
      }
    }
  }
}

// ---- Globals -----------------------------------------------------------------
GestureParams g_params;            // Live copy so calibration tools can tweak at runtime.
GestureEngine g_engine(g_params);  // Gesture interpreter built from the live parameters.
Sensor* g_sensor = nullptr;        // Assigned in setup() based on the compile-time flag.

// ---- Setup / Loop ------------------------------------------------------------
/**
 * Arduino entry point. We boot MIDI, select the concrete sensor, and emit a
 * JSON hello so any connected classroom tooling knows the firmware is ready.
 */
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // Choose sensor path
  #ifdef SENSOR_OPTICAL
    static OpticalSensor sensor_impl;
  #elif defined(SENSOR_CAPACITIVE)
    static CapacitiveSensorCalibrated sensor_impl;
  #elif defined(SENSOR_MAKEY)
    static MakeySensorGuarded sensor_impl;
  #elif defined(SENSOR_TOF)
    static TimeOfFlightSensor sensor_impl;
  #elif defined(SENSOR_PIEZO)
    static PiezoSensor sensor_impl;
  #endif
  g_sensor = &sensor_impl;
  g_sensor->begin();
  Serial.begin(SERIAL_BAUD);
  delay(500);
  Serial.println("{\"firmware\":\"StringField\",\"version\":\"0.2-dev\",\"serial\":\"ready\"}");
  Serial.println("{\"hint\":\"Send {\\\"notes\\\":[60,62,...]} + newline to hot-swap the scale. Type 'help' for this reminder.\"}");
}

/**
 * Main loop: poll serial (so commands stay snappy), read a sensor sample, let
 * the gesture engine classify it, and drive the MIDI + telemetry outputs. The
 * structure mirrors the teaching narrative: sense → classify → map → narrate.
 */
void loop() {
  pump_serial_commands();
  SensorSample s = g_sensor->read();
  Gesture g = g_engine.update(s);

  switch (g) {
    case Gesture::Pluck: {
      // Narration cue: "pluck → NoteOn velocity burst" — say it while showing the debugger.
      // Guard: don't stack overlapping notes
      if (current_note >= 0) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
      }
      current_note = next_note();
      uint8_t vel = (uint8_t)constrain(s.value * 127, 1, 127);
      MIDI.sendNoteOn(current_note, vel, kChannel);
      emit_gesture_event("pluck", vel, current_note);
      break;
    }
    case Gesture::Scrape: {
      // Narration cue: "scrape → micro-note grains at ~50 velocity".
      // Rapid small notes; velocity lower to read as grain
      uint8_t note = next_note();
      MIDI.sendNoteOn(note, 50, kChannel);
      MIDI.sendNoteOff(note, 0, kChannel);
      emit_gesture_event("scrape", 50, note);
      break;
    }
    case Gesture::Harmonic: {
      // Narration cue: "harmonic → glassy octave above"; we bias the pitch up so
      // students *hear* the light touch difference.
      if (current_note >= 0) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
      }
      uint8_t base = next_note();
      uint8_t harmonic_note = (uint8_t)constrain(base + 12, 0, 127);
      current_note = harmonic_note;
      uint8_t vel = 96;
      MIDI.sendNoteOn(harmonic_note, vel, kChannel);
      emit_gesture_event("harmonic", vel, harmonic_note);
      break;
    }
    case Gesture::Muted: {
      // Narration cue: "mute → note-off + short whisper". Great for damping riffs in class.
      if (current_note >= 0) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
        emit_gesture_event("mute", 0, current_note);
        current_note = -1;
      }
      break;
    }
    case Gesture::Tremolo: {
      // Quick amplitude wobbles: map to Expression so synths get a trembling loudness lane.
      uint8_t cc = (uint8_t)constrain(s.value * 127, 0, 127);
      MIDI.sendControlChange(11, cc, kChannel);
      emit_gesture_event("tremolo", cc, current_note);
      break;
    }
    case Gesture::Vibrato: {
      // Deeper wobble: swing pitch bend around center. Teensy MIDI uses +/-8192 range.
      int bend = (int)((s.value - 0.5f) * 2.0f * 8191);  // center on 0
      MIDI.sendPitchBend(bend, kChannel);
      emit_gesture_event("vibrato", (uint8_t)constrain(s.value * 127, 0, 127), current_note);
      break;
    }
    case Gesture::Bow: {
      // Narration cue: "bow → CC1 envelope stream"; invite students to map it to filters.
      // Continuous control (mod wheel)
      uint8_t cc = (uint8_t)constrain(s.value * 127, 0, 127);
      MIDI.sendControlChange(1, cc, kChannel);
      if (abs((int)cc - (int)last_bow_cc) > 2) {
        emit_gesture_event("bow", cc, current_note);
        last_bow_cc = cc;
      }
      break;
    }
    case Gesture::Idle: default:
      // If contact ended, release sustained note
      if (current_note >= 0 && s.value < g_params.off_thresh) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
        emit_gesture_event("release", 0, current_note);
        current_note = -1;
      }
      break;
  }
  pump_serial_commands();
}
