#include <math.h>

#include "sensor.h"

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

Sensor& get_piezo_sensor() {
  static PiezoSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_PIEZO

