#include "sensor.h"

#if defined(SENSOR_OPTICAL)
Sensor& get_optical_sensor();
#elif defined(SENSOR_CAPACITIVE)
Sensor& get_capacitive_sensor();
#elif defined(SENSOR_MAKEY)
Sensor& get_makey_sensor();
#elif defined(SENSOR_TOF)
Sensor& get_time_of_flight_sensor();
#elif defined(SENSOR_PIEZO)
Sensor& get_piezo_sensor();
#elif defined(SENSOR_PIR)
Sensor& get_pir_sensor();
#elif defined(SENSOR_ELECTRET)
Sensor& get_electret_sensor();
#elif defined(SENSOR_I2S_MIC)
Sensor& get_i2s_mic_sensor();
#endif

Sensor* make_sensor() {
#if defined(SENSOR_OPTICAL)
  return &get_optical_sensor();
#elif defined(SENSOR_CAPACITIVE)
  return &get_capacitive_sensor();
#elif defined(SENSOR_MAKEY)
  return &get_makey_sensor();
#elif defined(SENSOR_TOF)
  return &get_time_of_flight_sensor();
#elif defined(SENSOR_PIEZO)
  return &get_piezo_sensor();
#elif defined(SENSOR_PIR)
  return &get_pir_sensor();
#elif defined(SENSOR_ELECTRET)
  return &get_electret_sensor();
#elif defined(SENSOR_I2S_MIC)
  return &get_i2s_mic_sensor();
#else
  return nullptr;
#endif
}

