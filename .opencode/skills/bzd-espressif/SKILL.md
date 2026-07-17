---
name: bzd-espressif
description: How to build/test esp32/esp32s3/etc targets in the bzd monorepo.
compatibility: opencode
---

## Hardware & Silicon Documentation

- **Official Chip Datasheet (PDF):** [Espressif ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf) — _Pinouts, electrical characteristics, and hardware specifications._
- **Technical Reference Manual (PDF):** [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf) — _Detailed architectural information on registers, peripherals (I2C, SPI, UART), and internal memory._
- **Hardware Design Guidelines:** [Espressif Hardware Design Guide](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32/) — _Schematics, PCB layout constraints, and antenna design rules._

## Software Frameworks & API References

- **ESP-IDF (Official C/C++ Framework):** [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/) — _The comprehensive API reference for the official Espressif IoT Development Framework._
- **ESP-IDF (Official Rust Framework):** [ESP-IDF Rust](https://docs.espressif.com/projects/rust/)

## QEMU

- https://github.com/espressif/esp-toolchain-docs/tree/main/qemu/

## Commands

Useful commands to build, run, debug esp32 targets.

```bash
# Build and run a target for the esp32 platform on qemu.
./tools/bazel run //rust/esp32 --config=esp32

# Build and run a target for the esp32s3 platform on qemu.
./tools/bazel run //rust/esp32 --config=esp32s3

# Build a target for the esp32s platform with debug symbols and run it on qemu. Attach a gdb debug and wait for user commands.
./tools/bazel run //rust/esp32 --config=esp32 --config=gdb --config=dev

# When too many qemu process are ghosted.
killall qemu-system-xtensa
```
