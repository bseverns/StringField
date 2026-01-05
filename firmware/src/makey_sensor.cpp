#include "sensor.h"

#ifdef SENSOR_MAKEY

class MakeySensorGuarded : public Sensor {
 public:
  void begin() override { pinMode(kMakeyPin, INPUT_PULLUP); }

  SensorSample read() override {
    uint32_t now = micros();
    if (now - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now;

    // Expected signal range: digital gate with pullup enabled, so HIGH ~= 1 (open)
    // and LOW ~= 0 (touch shorts to ground).
    bool touch = digitalRead(kMakeyPin) == LOW;  // MaKey shorts to ground
    // Debounce using a tiny integrator so hand jitter does not look like a bow.
    debounce_state_ = 0.9f * debounce_state_ + 0.1f * (touch ? 1.0f : 0.0f);
    // Normalization: convert the debounced gate into a 0..1 envelope the rest of
    // the gesture engine can treat like any other sensor.
    float normalized = debounce_state_;
    // Common failure modes: no shared ground (always HIGH), too-long leads acting
    // like antennas (phantom touches), or ESD spikes that look like micro taps.

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

Sensor& get_makey_sensor() {
  static MakeySensorGuarded sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_MAKEY
