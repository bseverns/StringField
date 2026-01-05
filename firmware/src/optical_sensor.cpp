#include "sensor.h"

#ifdef SENSOR_OPTICAL

class OpticalSensor : public Sensor {
 public:
  void begin() override {
    // INTENT: keep pin assignments centralized; swapping pins should not touch logic.
    pinMode(A0, INPUT);    // phototransistor on A0 (via resistor)
    pinMode(13, OUTPUT);   // onboard LED for heartbeat
  }

  SensorSample read() override {
    // Expected signal range: analog 0..1023 from a phototransistor divider.
    // If you see 0 or 1023 all the time, check wiring and whether the sensor is saturated.
    int raw = analogRead(A0);  // 0..1023 on Teensy (will be 12-bit on some MCUs)
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);

    // Ambient light baseline: follow slow changes in the room without chasing the hand.
    if (x < ambient_floor_) {
      ambient_floor_ = 0.95f * ambient_floor_ + 0.05f * x;  // drop quickly if the room gets darker
    } else {
      ambient_floor_ = 0.995f * ambient_floor_ + 0.005f * x;  // rise slowly so hands don't become "baseline"
    }

    // Normalize: subtract the ambient floor, overdrive slightly for expressive motion.
    float normalized = (x - ambient_floor_) * gain_;
    normalized = constrain(normalized, 0.0f, 1.0f);

    // Low-pass smoothing to tame flicker without erasing intentional motion.
    env_ = 0.9f * env_ + 0.1f * normalized;

    // Common failure modes: sunlight swamping the sensor, reflective surfaces
    // causing false positives, or the emitter LED wired backward (flatline).
    digitalWrite(13, (millis() >> 6) & 1);  // slow blink to show life
    return {env_, micros()};
  }

 private:
  float ambient_floor_ = 0.0f;
  float env_ = 0.0f;
  const float gain_ = 1.4f;  // bump for low-contrast rooms; adjust in class
};

Sensor& get_optical_sensor() {
  static OpticalSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_OPTICAL
