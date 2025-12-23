#pragma once

#include <stdint.h>

#include <algorithm>
#include <cmath>

struct SensorSample {
  float value;
  uint32_t micros;
};

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

class GestureEngine {
 public:
  explicit GestureEngine(const GestureParams& p);
  Gesture update(const SensorSample& s);

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

