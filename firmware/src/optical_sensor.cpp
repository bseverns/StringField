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
    int raw = analogRead(A0);  // 0..1023 on Teensy (will be 12-bit on some MCUs)
    // Map raw to 0..1 with a crude low-pass to tame flicker; tune in calibration.
    static float y = 0.0f;
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    y = 0.9f * y + 0.1f * x;
    digitalWrite(13, (millis() >> 6) & 1);  // slow blink to show life
    return {y, micros()};
  }
};

Sensor& get_optical_sensor() {
  static OpticalSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_OPTICAL

