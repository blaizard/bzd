"""Metadata for ESP32 toolchains."""

load("//:fragments/esp32/esp32_xtensa_lx6_gcc_11.2.0/defs.bzl", "esp32_xtensa_lx6_gcc_11_2_0")
load("//:fragments/esp32/esp32_xtensa_lx6_gcc_12.2.0/defs.bzl", "esp32_xtensa_lx6_gcc_12_2_0")

esp32 = {
    "esp32_gcc_11.2.0": {
        "esp32-xtensa_lx6": esp32_xtensa_lx6_gcc_11_2_0,
    },
    "esp32_gcc_12.2.0": {
        "esp32-xtensa_lx6": esp32_xtensa_lx6_gcc_12_2_0,
    },
}
