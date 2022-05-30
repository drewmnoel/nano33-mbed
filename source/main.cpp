#include "WeatherBeacon.h"
#include "nano_errata.h"
#include <ble/BLE.h>
#include <mbed.h>

DigitalOut led(LED1);
void blink();

events::EventQueue event_queue;

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
  event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

void blink() { led = !led; }

int main() {
  fix_arduino_bootloader();
  event_queue.call_every(1s, blink);

  BLE &ble = BLE::Instance();
  ble.onEventsToProcess(schedule_ble_events);
  WeatherBeacon WeatherBeacon(ble, event_queue);
  WeatherBeacon.run();
  return 0;
}