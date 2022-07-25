# Compiling

1. `sudo pacman -S arm-none-eabi-newlib arm-none-eabi-gcc arm-none-eabi-binutils`
2. Set up `mbed` somehow?
3. `mbed compile -m ARDUINO_NANO33BLE -t GCC_ARM`
4. `bossac -U -i -e -w ./BUILD/ARDUINO_NANO33BLE/GCC_ARM/appnamehere.bin -R` (using Arduino's `bossac` tweak)

