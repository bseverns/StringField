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

#include "sensor.h"

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

// ---- Gesture Engine ----------------------------------------------------------
// INTENT: Detect three coarse gestures using only a thresholded stream. Keep
// all constants named and easy to calibrate.
/**
 * Tunable gesture thresholds. Treat this like a calibration worksheet: these
 * numbers start as defaults and should be tweaked with students while watching
 * the debugger. The hysteresis (`on_thresh` / `off_thresh`) keeps the contact
 * state stable; the time windows set how fast is "scrape" versus a fresh pluck.
 */
struct GestureParams {
  float on_thresh = 0.55f;        // crossing above => "contact"
  float off_thresh = 0.40f;       // falling below => "release"
  uint32_t min_retrigger_us = 60000;  // 60ms guard against double plucks
  uint32_t scrape_window_us = 40000;  // <40ms between mini-onsets => scrape grain

  // Extended gestures (all narratable in class; edit + reflash or push via Serial)
  float harmonic_peak_min = 0.35f;    // light touch floor; tweak while listening for chime partials
  float harmonic_peak_max = 0.65f;    // light touch ceiling
  uint32_t harmonic_hold_us = 70000;  // hold a light touch this long → call it harmonic
  float harmonic_variation_eps = 0.05f;  // how still the envelope must be to count as a harmonic touch

  float mute_peak_thresh = 0.25f;     // low-amplitude contacts that end early => muted articulation
  uint32_t mute_window_us = 50000;    // release within this window → treat as mute instead of pluck-off
  float mute_release_thresh = 0.15f;  // if the release falls below this before rising again, call it a mute

  float tremolo_min_delta = 0.06f;    // minimum swing to count as one wobble
  uint32_t tremolo_max_period_us = 30000;  // how fast the sign flips must be (<33 Hz)
  uint32_t tremolo_grace_us = 8000;        // ignore micro-wobbles right at onset; gives players a breath
  uint8_t wobble_goal = 4;            // how many flips before we declare tremolo/vibrato
  float vibrato_depth_min = 0.15f;    // deeper swings ⇒ vibrato; shallow ⇒ tremolo
};

enum class Gesture { Idle, Pluck, Bow, Scrape, Harmonic, Muted, Tremolo, Vibrato };

/**
 * Converts a stream of SensorSamples into semantic gestures. The design goal
 * is to keep the rules audible and debuggable. There is no hidden machine
 * learning here—just explicit timing and hysteresis—so a class can reason about
 * why a particular motion produced "bow" versus "pluck".
 */
class GestureEngine {
 public:
  GestureEngine(const GestureParams& p) : p_(p) {}
  Gesture update(const SensorSample& s) {
    // Hysteresis for contact state
    bool prev = contact_;
    if (!contact_ && s.value >= p_.on_thresh) contact_ = true;
    if (contact_ && s.value <= p_.off_thresh) contact_ = false;

    // Update per-contact tracking for extended gestures
    if (contact_ && !prev) {
      contact_start_us_ = s.micros;
      peak_value_ = s.value;
      wobble_min_ = s.value;
      wobble_max_ = s.value;
      wobble_count_ = 0;
      last_wobble_us_ = s.micros;
      last_direction_up_ = true;
      harmonic_called_ = false;
      modulation_called_ = false;
      mute_candidate_ = false;
      contact_state_ = ContactState::Attacking;
    }
    if (contact_) {
      peak_value_ = max(peak_value_, s.value);
      wobble_min_ = min(wobble_min_, s.value);
      wobble_max_ = max(wobble_max_, s.value);
      if (contact_state_ == ContactState::Attacking &&
          (s.micros - contact_start_us_) > p_.tremolo_grace_us) {
        contact_state_ = ContactState::Sustaining;
      }
    }

    // Tremolo/vibrato wobble detection: count sign changes when the swing is loud enough.
    float delta = s.value - last_value_;
    bool rising = delta >= 0.0f;
    if (fabs(delta) >= p_.tremolo_min_delta && contact_state_ == ContactState::Sustaining) {
      if (rising != last_direction_up_) {
        uint32_t wobble_dt = s.micros - last_wobble_us_;
        if (wobble_dt <= p_.tremolo_max_period_us) {
          ++wobble_count_;
        } else {
          wobble_count_ = 1;  // reset if too slow; still count current flip
        }
        last_wobble_us_ = s.micros;
        last_direction_up_ = rising;
      }
    }
    last_value_ = s.value;

    // Onset detection
    Gesture g = Gesture::Idle;
    if (contact_ && !prev) {
      uint32_t dt = s.micros - last_onset_us_;
      if (dt < p_.min_retrigger_us) {
        g = Gesture::Idle;  // debounce double strike
      } else {
        g = (dt < p_.scrape_window_us) ? Gesture::Scrape : Gesture::Pluck;
        last_onset_us_ = s.micros;
      }
    } else if (contact_) {
      // Harmonics: require a light touch that stays stable. This prevents loud plucks
      // from being mis-labeled when the player lingers.
      bool in_harmonic_band = peak_value_ >= p_.harmonic_peak_min && peak_value_ <= p_.harmonic_peak_max;
      float wobble_depth = wobble_max_ - wobble_min_;
      if (!harmonic_called_ && in_harmonic_band && wobble_depth <= p_.harmonic_variation_eps &&
          (s.micros - contact_start_us_) > p_.harmonic_hold_us) {
        harmonic_called_ = true;
        g = Gesture::Harmonic;
      } else if (!modulation_called_ && wobble_count_ >= p_.wobble_goal) {
        modulation_called_ = true;  // only announce once per contact so the serial log stays readable
        g = (wobble_depth >= p_.vibrato_depth_min) ? Gesture::Vibrato : Gesture::Tremolo;
      } else {
        // Sustained contact → treat as bow (continuous control)
        g = Gesture::Bow;
      }
      // Track whether the player is drifting toward a mute: low peak and falling envelope.
      if (peak_value_ <= p_.mute_peak_thresh && s.value <= p_.mute_release_thresh) {
        mute_candidate_ = true;
      }
    } else if (prev && !contact_) {
      contact_state_ = ContactState::Released;
      uint32_t hold = s.micros - contact_start_us_;
      // Quick-touch deadening => mute gesture; otherwise idle falls through to release handling in loop().
      if (hold <= p_.mute_window_us || mute_candidate_) {
        g = Gesture::Muted;
      }
    }
    return g;
  }

 private:
    enum class ContactState { Released, Attacking, Sustaining };
    GestureParams p_;
    bool contact_ = false;
    uint32_t last_onset_us_ = 0;
    uint32_t contact_start_us_ = 0;
    float peak_value_ = 0.0f;
    float last_value_ = 0.0f;
    float wobble_min_ = 1.0f;
    float wobble_max_ = 0.0f;
    uint8_t wobble_count_ = 0;
    uint32_t last_wobble_us_ = 0;
    bool last_direction_up_ = true;
    bool harmonic_called_ = false;
    bool modulation_called_ = false;
    bool mute_candidate_ = false;
    ContactState contact_state_ = ContactState::Released;
};

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
  g_sensor = make_sensor();
  if (g_sensor != nullptr) {
    g_sensor->begin();
  }
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
  if (g_sensor == nullptr) return;
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
