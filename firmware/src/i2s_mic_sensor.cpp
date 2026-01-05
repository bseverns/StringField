#include "sensor.h"

#ifdef SENSOR_I2S_MIC
#include <I2S.h>

class I2SMicSensor : public Sensor {
 public:
  void begin() override {
    // Default to 16 kHz mono; adjust for your part. If begin() fails, keep
    // reading zeros so the rest of the firmware stays predictable in class.
    if (!I2S.begin(I2S_PHILIPS_MODE, sample_rate_, bits_)) {
      ready_ = false;
    } else {
      ready_ = true;
    }
  }

  SensorSample read() override {
    uint32_t now = micros();
    if (!ready_) return {0.0f, now};

    int32_t total = 0;
    int16_t sample = 0;
    uint16_t count = 0;
    while (I2S.available() && count < window_samples_) {
      // Expected signal range: signed 16-bit PCM samples (about -32768..32767).
      sample = static_cast<int16_t>(I2S.read());
      total += abs(sample);
      ++count;
    }

    float avg = (count > 0) ? (total / static_cast<float>(count)) : 0.0f;
    // Normalize 16-bit PCM to 0..1 and overdrive slightly for quiet rooms.
    float normalized = constrain((avg / 32768.0f) * gain_, 0.0f, 1.0f);
    // Normalization: absolute-value average -> 0..1 energy envelope with a short
    // smoothing filter so percussive taps still show up as peaks.
    env_ = 0.9f * env_ + 0.1f * normalized;
    // Common failure modes: I2S.begin() never locks (ready_ false => silence),
    // word-select swapped (garbled noise), or sample_rate_ mismatch (aliasy hiss).
    return {env_, now};
  }

 private:
  const int sample_rate_ = 16000;
  const int bits_ = 16;
  const uint16_t window_samples_ = 64;  // ~4 ms at 16 kHz; short for fast articulation
  const float gain_ = 2.5f;

  bool ready_ = false;
  float env_ = 0.0f;
};

Sensor& get_i2s_mic_sensor() {
  static I2SMicSensor sensor_impl;
  return sensor_impl;
}

#endif  // SENSOR_I2S_MIC
