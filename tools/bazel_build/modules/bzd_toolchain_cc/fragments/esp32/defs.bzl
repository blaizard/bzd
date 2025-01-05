"""Metadata for ESP32 toolchains."""

load("//:fragments/esp32/esp32_xtensa_lx6_gcc_14.2.0/esp32.bzl", "esp32_xtensa_lx6_gcc_14_2_0")

esp32 = {
    "esp32_gcc_14.2.0": {
        "esp32-xtensa_lx6": esp32_xtensa_lx6_gcc_14_2_0,
    },
}
