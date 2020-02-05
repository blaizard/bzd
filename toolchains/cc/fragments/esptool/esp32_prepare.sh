#!/bin/bash --norc

exec external/esptool/esptool.py --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "2MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1"
