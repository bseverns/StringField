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

// ---- Compile-time selection of sensing path ---------------------------------
// Define exactly one of these in platformio.ini build_flags, e.g. -D SENSOR_OPTICAL
#if !defined(SENSOR_OPTICAL) && !defined(SENSOR_CAPACITIVE) && !defined(SENSOR_MAKEY)
  #define SENSOR_OPTICAL 1  // default demo
#endif

// ---- MIDI setup --------------------------------------------------------------
MIDI_CREATE_DEFAULT_INSTANCE();
static const uint8_t kChannel = 1;  // 1-16

// ---- Shared types ------------------------------------------------------------
struct SensorSample {
  float value;      // normalized 0..1 (modality-specific scaling happens underneath)
  uint32_t micros;  // timestamp for gesture timing
};

// Abstract sensor interface so gesture code doesn't care which hardware we use.
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

// ---- Capacitive / MaKey stubs ------------------------------------------------
#ifdef SENSOR_CAPACITIVE
class CapacitiveSensorDemo : public Sensor {
 public:
  void begin() override { pinMode(A1, INPUT); }
  SensorSample read() override {
    // Placeholder: replace with proper capacitive read (e.g., charge/discharge timing or IC)
    int raw = analogRead(A1);
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    return {x, micros()};
  }
};
#endif

#ifdef SENSOR_MAKEY
class MakeySensorDemo : public Sensor {
 public:
  void begin() override { pinMode(2, INPUT_PULLUP); }
  SensorSample read() override {
    bool touched = digitalRead(2) == LOW;
    return {touched ? 1.0f : 0.0f, micros()};
  }
};
#endif

// ---- Gesture Engine ----------------------------------------------------------
// INTENT: Detect three coarse gestures using only a thresholded stream. Keep
// all constants named and easy to calibrate.
struct GestureParams {
  float on_thresh = 0.55f;        // crossing above => "contact"
  float off_thresh = 0.40f;       // falling below => "release"
  uint32_t min_retrigger_us = 60000;  // 60ms guard against double plucks
  uint32_t scrape_window_us = 40000;  // <40ms between mini-onsets => scrape grain
};

enum class Gesture { Idle, Pluck, Bow, Scrape };

class GestureEngine {
 public:
  GestureEngine(const GestureParams& p) : p_(p) {}
  Gesture update(const SensorSample& s) {
    // Hysteresis for contact state
    bool prev = contact_;
    if (!contact_ && s.value >= p_.on_thresh) contact_ = true;
    if (contact_ && s.value <= p_.off_thresh) contact_ = false;

    // Onset detection
    Gesture g = Gesture::Idle;
    if (contact_ && !prev) {
      uint32_t dt = s.micros - last_onset_us_;
      g = (dt < p_.scrape_window_us) ? Gesture::Scrape : Gesture::Pluck;
      last_onset_us_ = s.micros;
    } else if (contact_) {
      // Sustained contact → treat as bow (continuous control)
      g = Gesture::Bow;
    }
    return g;
  }

 private:
    GestureParams p_;
    bool contact_ = false;
    uint32_t last_onset_us_ = 0;
};

// ---- Mapping -----------------------------------------------------------------
// Map gestures to MIDI: notes from a small pentatonic set, CC1 for bow energy.
struct NoteSet {
  // C major pentatonic by default; can be replaced by JSON at runtime in future.
  uint8_t notes[5] = {60, 62, 64, 67, 69}; // C D E G A
  uint8_t size = 5;
  uint8_t idx = 0;
} g_notes;

uint8_t next_note() {
  uint8_t n = g_notes.notes[g_notes.idx];
  g_notes.idx = (g_notes.idx + 1) % g_notes.size;
  return n;
}

// ---- Globals -----------------------------------------------------------------
GestureParams g_params;
GestureEngine g_engine(g_params);
Sensor* g_sensor = nullptr;
int current_note = -1;

// ---- Setup / Loop ------------------------------------------------------------
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // Choose sensor path
  #ifdef SENSOR_OPTICAL
    static OpticalSensor sensor_impl;
  #elif defined(SENSOR_CAPACITIVE)
    static CapacitiveSensorDemo sensor_impl;
  #elif defined(SENSOR_MAKEY)
    static MakeySensorDemo sensor_impl;
  #endif
  g_sensor = &sensor_impl;
  g_sensor->begin();
  Serial.begin(SERIAL_BAUD);
  delay(500);
  Serial.println("StringField firmware v0.1 starting...");
}

void loop() {
  SensorSample s = g_sensor->read();
  Gesture g = g_engine.update(s);

  switch (g) {
    case Gesture::Pluck: {
      // Guard: don't stack overlapping notes
      if (current_note >= 0) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
      }
      current_note = next_note();
      uint8_t vel = (uint8_t)constrain(s.value * 127, 1, 127);
      MIDI.sendNoteOn(current_note, vel, kChannel);
      break;
    }
    case Gesture::Scrape: {
      // Rapid small notes; velocity lower to read as grain
      uint8_t note = next_note();
      MIDI.sendNoteOn(note, 50, kChannel);
      MIDI.sendNoteOff(note, 0, kChannel);
      break;
    }
    case Gesture::Bow: {
      // Continuous control (mod wheel)
      uint8_t cc = (uint8_t)constrain(s.value * 127, 0, 127);
      MIDI.sendControlChange(1, cc, kChannel);
      break;
    }
    case Gesture::Idle: default:
      // If contact ended, release sustained note
      if (current_note >= 0 && s.value < g_params.off_thresh) {
        MIDI.sendNoteOff(current_note, 0, kChannel);
        current_note = -1;
      }
      break;
  }
}
