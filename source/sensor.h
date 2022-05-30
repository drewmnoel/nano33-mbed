#ifndef _SENSOR_H_
#define _SENSOR_H_

// Struct to hold AHT20 sensor data
typedef struct {
  float temperature;
  float humidity;
} sensor_result_t;

void setup_sensor();
sensor_result_t get_measurement();

#endif