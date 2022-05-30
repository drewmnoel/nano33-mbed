#include "WeatherBeacon.h"
#include <ble/BLE.h>
#include <ble/Gap.h>
#include <mbed.h>

#define BEACON_NAME "WS: KTCH"

/** Start BLE interface initialisation */
void WeatherBeacon::run(void) {
  if (_ble.hasInitialized()) {
    printf("Ble instance already initialised.\r\n");
    return;
  }

  ble_error_t error = _ble.init(this, &WeatherBeacon::on_init_complete);
  if (error) {
    return;
  }

  _event_queue.call_every(750ms, this, &WeatherBeacon::process_data);
  _event_queue.dispatch_forever();
}

/** This is called when BLE interface is initialised and starts the first mode
 */
void WeatherBeacon::on_init_complete(
    BLE::InitializationCompleteCallbackContext *event) {
  if (event->error) {
    return;
  }

  if (!_gap.isFeatureSupported(
          ble::controller_supported_features_t::LE_EXTENDED_ADVERTISING) ||
      !_gap.isFeatureSupported(
          ble::controller_supported_features_t::LE_PERIODIC_ADVERTISING)) {
    return;
  }

  /* all calls are serialised on the user thread through the event queue */
  _event_queue.call(this, &WeatherBeacon::advertise);
}

/** Set up and start advertising */
void WeatherBeacon::advertise(void) {
  static_assert(STR(BEACON_NAME) < 16);

  ble::AdvertisingParameters adv_params(
      ble::advertising_type_t::NON_CONNECTABLE_UNDIRECTED,
      ble::adv_interval_t::min(), ble::adv_interval_t::max(), true);

  adv_params.setOwnAddressType(ble::own_address_type_t::RANDOM);
  adv_params.setTxPower(8);

  if (_adv_handle == ble::INVALID_ADVERTISING_HANDLE) {
    _gap.createAdvertisingSet(&_adv_handle, adv_params);
  }

  _adv_data_builder.clear();
  _adv_data_builder.setFlags();
  _adv_data_builder.setName(BEACON_NAME);

  _gap.setAdvertisingParameters(_adv_handle, adv_params);
  _gap.setAdvertisingPayload(_adv_handle,
                             _adv_data_builder.getAdvertisingData());
  _gap.startAdvertising(_adv_handle);

  printf("Advertising started!\r\n");
}

// Update the GAP advertising payload tuple
void WeatherBeacon::update_payload(float temp, float humid) {
  uint8_t *temp_u = reinterpret_cast<uint8_t *>(&temp);
  uint8_t *humi_u = reinterpret_cast<uint8_t *>(&humid);
  uint8_t data_bytes[8];

  for (int i = 0; i < 4; i++) {
    data_bytes[i] = temp_u[i];
    data_bytes[i + 4] = humi_u[i];
  }

  Span<const uint8_t> span_from_array(data_bytes);

  _adv_data_builder.setServiceData(GattService::UUID_ENVIRONMENTAL_SERVICE,
                                   span_from_array);

  _gap.setAdvertisingPayload(_adv_handle,
                             _adv_data_builder.getAdvertisingData());

  _gap.setPeriodicAdvertisingPayload(_adv_handle,
                                     _adv_data_builder.getAdvertisingData());
}

void WeatherBeacon::process_data() {
  sensor_result_t res = get_measurement();
  update_payload(res.temperature, res.humidity);
  wait_us(100*1000);
}
