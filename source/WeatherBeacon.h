#ifndef _WEATHERBEACON_H_
#define _WEATHERBEACON_H_
#include "sensor.h"
#include <ble/BLE.h>
#include <ble/Gap.h>
#include <mbed.h>

template<std::size_t N>
constexpr size_t STR(char const (&s)[N]) {
    return N;
}

class WeatherBeacon {
public:
  static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 59;
  WeatherBeacon(BLE &ble, events::EventQueue &event_queue)
      : _ble(ble), _gap(ble.gap()), _event_queue(event_queue),
        _adv_data_builder(_adv_buffer),
        _adv_handle(ble::INVALID_ADVERTISING_HANDLE) {}

  ~WeatherBeacon() {
    if (_ble.hasInitialized()) {
      _ble.shutdown();
    }
  }

  void run();

private:
  void on_init_complete(BLE::InitializationCompleteCallbackContext *event);
  void startScanForever(void);
  void advertise(void);
  void update_payload(float, float);
  void update_sensor_value(void);
  void process_data(void);

private:
  BLE &_ble;
  ble::Gap &_gap;
  events::EventQueue &_event_queue;

  uint8_t _adv_buffer[MAX_ADVERTISING_PAYLOAD_SIZE];
  ble::AdvertisingDataBuilder _adv_data_builder;

  ble::advertising_handle_t _adv_handle;
};
#endif