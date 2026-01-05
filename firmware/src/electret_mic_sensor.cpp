#include <math.h>

#include "sensor.h"

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
    // Expected signal range: biased mic envelope on A4, sampled 0..1023. You want
    // mid-rail idle (around 0.5 normalized) so the swing has room both ways.
    float x = sample_raw();
    // AC coupling: follow bias slowly, measure swing fast.
    bias_ = 0.9994f * bias_ + 0.0006f * x;  // tweak live if the room hums
    float swing = fabs(x - bias_) * gain_;
    // Normalization behavior: clamp the rectified swing into 0..1, then smooth to a
    // performance-friendly envelope.
    env_ = 0.88f * env_ + 0.12f * constrain(swing, 0.0f, 1.0f);
    // Small floor clamp to avoid whisper-noise. Bump lower if you need pianissimo.
    if (env_ < 0.01f) env_ = 0.0f;
    // Common failure modes: missing bias resistor (floating pin noise),
    // long unshielded leads (50/60 Hz hum), or gain too high (perma-clipped 1.0).
    return {env_, now};
  }

 private:
  static const uint8_t kMicPin = A4;  // analog envelope input; keep wiring short
  const float gain_ = 3.5f;           // adjust alongside bias speed during calibration

  float bias_ = 0.5f;
  float env_ = 0.0f;

  float sample_raw() { return constrain(analogRead(kMicPin) / 1023.0f, 0.0f, 1.0f); }
};

Sensor& get_electret_sensor() {
  static ElectretMicSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_ELECTRET
