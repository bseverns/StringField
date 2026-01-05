#include "sensor.h"

#ifdef SENSOR_TOF

class TimeOfFlightSensor : public Sensor {
 public:
  void begin() override {
    // Pin/PWR notes: real ToF breakouts (VL53L0X, TMF8801, etc.) use I2C. Here we
    // map the analog envelope from a helper board onto A2 to keep the demo
    // solderable and readable in-class. Swap the pin when moving to a true I2C
    // driver; keep the function shape identical so GestureEngine stays agnostic.
    pinMode(A2, INPUT);
  }

  SensorSample read() override {
    // Expected signal range: analog envelope 0..1023 from an external ToF helper
    // (or later: millimeters via I2C). If you see rails at 0/1023, check wiring
    // and whether the sensor is saturating in sunlight.
    int raw = analogRead(A2);  // e.g., 0..1023. Replace with mm reading / 4096.0 for I2C parts.
    // Calibrate like a lab notebook: expose the bias and smoothing knobs. Students
    // can anchor a “hand at 15 cm” pose and tune the filter and scaling live.
    static float y = 0.0f;
    // Normalization: map raw ADC to 0..1 so gesture logic stays sensor-agnostic.
    float x = constrain(raw / 1023.0f, 0.0f, 1.0f);
    y = 0.85f * y + 0.15f * x;  // slightly faster than optical to catch hand waves
    // Cheap floor clamp for noisy rooms; edit in class if your sensor never
    // truly rests at 0.0 while idle.
    if (y < 0.02f) y = 0.0f;
    // Common failure modes: ambient IR swamping the receiver (always high),
    // over-aggressive RC filtering (sluggish bow response), or A2 floating
    // because the analog helper board isn't powered.
    return {y, micros()};
  }
};

Sensor& get_time_of_flight_sensor() {
  static TimeOfFlightSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_TOF
