#include "sensor.h"
#include "AHT20.h"
#include <mbed.h>

#define I2C_FREQ 100000UL
#define AHT20_DELAY 80000UL

AHT20 primaryAht(I2C_SDA0, I2C_SCL0, AHT20::AHT20_ADDRESS, I2C_FREQ);
AHT20::AHT20_user_data_t primaryAht_Data;

void setup_sensor() {
  // Reset the device
  primaryAht.AHT20_SoftReset();
  wait_us(AHT20_DELAY);

  // Get the state byte
  primaryAht.AHT20_TriggerStatus();
  wait_us(AHT20_DELAY);

  primaryAht.AHT20_GetStatus((uint8_t *)&primaryAht_Data.state);

  // Only calibrate once
  if ((primaryAht_Data.state & AHT20::STATUS_CAL_MASK) ==
      AHT20::STATUS_CAL_UNCALIBRATED) {
    primaryAht.AHT20_Calibrate();
  }
}

sensor_result_t get_measurement() {
  // Request a measurement
  primaryAht.AHT20_TriggerMeasurement();
  wait_us(AHT20_DELAY);

  // Read from the device
  primaryAht.AHT20_GetAllData((AHT20::AHT20_user_data_t *)&primaryAht_Data);

  // Normalize the data
  primaryAht_Data.temperature.temperature = primaryAht.AHT20_ProcessTemperature(
      primaryAht_Data.temperature.raw_temperature);
  primaryAht_Data.humidity.humidity =
      primaryAht.AHT20_ProcessHumidity(primaryAht_Data.humidity.raw_humidity);

  // Return the results as a struct
  sensor_result_t res;
  res.temperature = primaryAht_Data.temperature.temperature;
  res.humidity = primaryAht_Data.humidity.humidity;
  return res;
}