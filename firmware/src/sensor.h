#pragma once

#include <Arduino.h>

// ---- Compile-time selection of sensing path ---------------------------------
// Define exactly one of these in platformio.ini build_flags, e.g. -D SENSOR_OPTICAL
#if !defined(SENSOR_OPTICAL) && !defined(SENSOR_CAPACITIVE) && !defined(SENSOR_MAKEY) && !defined(SENSOR_TOF) && \
    !defined(SENSOR_PIEZO) && !defined(SENSOR_PIR) && !defined(SENSOR_ELECTRET) && !defined(SENSOR_I2S_MIC)
  #define SENSOR_OPTICAL 1  // default demo; explicitly include new options above
#endif

/**
 * A single sensor reading with the two pieces of data the gesture engine needs.
 */
struct SensorSample {
  float value;
  uint32_t micros;
};

/**
 * Abstract sensing interface. The firmware intentionally hides the details of
 * which physical stack is active so students can swap implementations without
 * rewriting the gesture logic.
 */
class Sensor {
 public:
  virtual void begin() = 0;
  virtual SensorSample read() = 0;
  virtual ~Sensor() {}
};

// Factory that returns the compile-time-selected sensor implementation.
Sensor* make_sensor();

