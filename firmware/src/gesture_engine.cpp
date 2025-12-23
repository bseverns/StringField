#include "gesture_engine.h"

#include <algorithm>
#include <cmath>

GestureEngine::GestureEngine(const GestureParams& p) : p_(p) {}

Gesture GestureEngine::update(const SensorSample& s) {
  bool prev = contact_;
  if (!contact_ && s.value >= p_.on_thresh) contact_ = true;
  if (contact_ && s.value <= p_.off_thresh) contact_ = false;

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
    peak_value_ = std::max(peak_value_, s.value);
    wobble_min_ = std::min(wobble_min_, s.value);
    wobble_max_ = std::max(wobble_max_, s.value);
    if (contact_state_ == ContactState::Attacking &&
        (s.micros - contact_start_us_) > p_.tremolo_grace_us) {
      contact_state_ = ContactState::Sustaining;
    }
  }

  float delta = s.value - last_value_;
  bool rising = delta >= 0.0f;
  if (std::fabs(delta) >= p_.tremolo_min_delta && contact_state_ == ContactState::Sustaining) {
    if (rising != last_direction_up_) {
      uint32_t wobble_dt = s.micros - last_wobble_us_;
      if (wobble_dt <= p_.tremolo_max_period_us) {
        ++wobble_count_;
      } else {
        wobble_count_ = 1;
      }
      last_wobble_us_ = s.micros;
      last_direction_up_ = rising;
    }
  }
  last_value_ = s.value;

  Gesture g = Gesture::Idle;
  if (contact_ && !prev) {
    uint32_t dt = s.micros - last_onset_us_;
    if (dt < p_.min_retrigger_us) {
      g = Gesture::Idle;
    } else {
      g = (dt < p_.scrape_window_us) ? Gesture::Scrape : Gesture::Pluck;
      last_onset_us_ = s.micros;
    }
  } else if (contact_) {
    bool in_harmonic_band = peak_value_ >= p_.harmonic_peak_min && peak_value_ <= p_.harmonic_peak_max;
    float wobble_depth = wobble_max_ - wobble_min_;
    if (!harmonic_called_ && in_harmonic_band && wobble_depth <= p_.harmonic_variation_eps &&
        (s.micros - contact_start_us_) > p_.harmonic_hold_us) {
      harmonic_called_ = true;
      g = Gesture::Harmonic;
    } else if (!modulation_called_ && wobble_count_ >= p_.wobble_goal) {
      modulation_called_ = true;
      g = (wobble_depth >= p_.vibrato_depth_min) ? Gesture::Vibrato : Gesture::Tremolo;
    } else {
      g = Gesture::Bow;
    }
    if (peak_value_ <= p_.mute_peak_thresh && s.value <= p_.mute_release_thresh) {
      mute_candidate_ = true;
    }
  } else if (prev && !contact_) {
    contact_state_ = ContactState::Released;
    uint32_t hold = s.micros - contact_start_us_;
    if (hold <= p_.mute_window_us || mute_candidate_) {
      g = Gesture::Muted;
    }
  }
  return g;
}

