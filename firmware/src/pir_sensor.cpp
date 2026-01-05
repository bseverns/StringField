#include "sensor.h"

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

    // Expected signal range: digital HIGH/LOW gate from the PIR comparator.
    bool raw_motion = digitalRead(kPirPin) == HIGH;
    float x = raw_motion ? 1.0f : 0.0f;
    // Normalization: convert the binary gate into a smoothed 0..1 envelope so
    // gestures feel like motion energy rather than a square wave.
    // Exponential decay so a single person pass shows as a hill, not a square wave.
    env_ = 0.92f * env_ + 0.08f * x;
    // Guard window to avoid rapid re-triggers from onboard comparators that chatter.
    if (now_us - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now_us;

    // Common failure modes: powering a 3.3 V-only PIR from 5 V (stuck high),
    // heat vents/sunlight causing false triggers, or mounting aimed at the floor
    // so it never sees lateral motion.
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

Sensor& get_pir_sensor() {
  static PirSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_PIR
