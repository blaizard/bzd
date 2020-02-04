#!/bin/bash --norc

exec external/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 toolchains/fragments/esptool/bin/esp32/bootloader.bin 0x10000 "$1" 0x8000 toolchains/fragments/esptool/bin/esp32/partitions_singleapp.bin
