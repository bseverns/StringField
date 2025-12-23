#include "sensor.h"

#ifdef SENSOR_CAPACITIVE

class CapacitiveSensorCalibrated : public Sensor {
 public:
  void begin() override {
    pinMode(kPadPin, INPUT);
    // Pre-fill the baseline with a quick average so the first loop iteration
    // does not spike. Students can watch this in the serial plotter.
    uint32_t sum = 0;
    for (int i = 0; i < 16; ++i) {
      sum += measure_raw();
      delay(2);
    }
    baseline_ = sum / 16.0f;
  }

  SensorSample read() override {
    uint32_t now = micros();
    // Guard: if a previous read happened too recently, reuse the last sample.
    if (now - last_read_us_ < guard_us_) return last_sample_;
    last_read_us_ = now;

    uint16_t raw = measure_raw();
    // Slow baseline drift follower; this resists humidity swings but keeps
    // quick touches visible. The rate is intentionally tiny for classroom calm.
    baseline_ = 0.999f * baseline_ + 0.001f * raw;
    float delta = raw - baseline_;
    float normalized = constrain(delta / sensitivity_scale_, 0.0f, 1.0f);
    last_sample_ = {normalized, now};
    return last_sample_;
  }

 private:
  static const uint8_t kPadPin = A1;  // pad → A1, driven against GND bracelet
  const uint16_t settle_us_ = 50;     // let the pullup charge the pad before sampling
  const uint16_t discharge_us_ = 200; // drain to ground so each read starts clean
  const uint16_t guard_us_ = 1000;    // 1ms guard between reads to avoid ghosting
  const float sensitivity_scale_ = 400.0f;  // tweak alongside on/off thresholds

  float baseline_ = 0.0f;
  uint32_t last_read_us_ = 0;
  SensorSample last_sample_{0.0f, 0};

  uint16_t measure_raw() {
    // Drain any residual charge. This assumes the performer is grounded via a
    // wrist strap or shared foil so the pad always has a reference.
    pinMode(kPadPin, OUTPUT);
    digitalWrite(kPadPin, LOW);
    delayMicroseconds(discharge_us_);

    // Charge and measure the RC rise time using the built-in ADC. On Teensy
    // this is ~10-bit; if your board has higher resolution, keep the 0..1023
    // normalize above and narrate the change.
    pinMode(kPadPin, INPUT_PULLUP);
    delayMicroseconds(settle_us_);
    return analogRead(kPadPin);
  }
};

Sensor& get_capacitive_sensor() {
  static CapacitiveSensorCalibrated sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_CAPACITIVE

