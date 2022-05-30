# Compiling

1. `sudo pacman -S arm-none-eabi-newlib arm-none-eabi-gcc arm-none-eabi-binutils`
2. Set up `mbed` somehow?
3. Override Nano target as below (`OUTPUT_EXT` and `mbed_app_start`)
4. `mbed compile -m ARDUINO_NANO33BLE -t GCC_ARM`
5. `bossac -U -i -e -w ./BUILD/ARDUINO_NANO33BLE/GCC_ARM/appnamehere.bin -R`

## Overridden `targets/targets.json`

```json
"ARDUINO_NANO33BLE": {
    "inherits": [
        "MCU_NRF52840"
    ],
    "features_add": [
        "STORAGE"
    ],
    "components_remove": [
        "QSPIF"
    ],
    "device_has_remove": [
        "QSPI",
        "ITM"
    ],
    "macros_add": [
        "CONFIG_GPIO_AS_PINRESET"
    ],
    "OUTPUT_EXT": "bin",
    "mbed_app_start": "0x10000"
}
```
