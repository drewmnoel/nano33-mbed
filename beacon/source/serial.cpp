#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "mbed.h"
#include "USBSerial.h"
#include "pretty_printer.h"
#include "mbed-trace/mbed_trace.h"

USBSerial serial(false);
FileHandle *mbed::mbed_override_console(int) {
  return &serial;
}

// Use: serial.connect();
// serial.wait_read();
