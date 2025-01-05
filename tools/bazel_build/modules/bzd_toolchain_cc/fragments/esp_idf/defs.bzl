"""Metadata for ESP32 toolchains."""

load("//:fragments/esp_idf/xtensa_esp_elf_14.2.0/esp32.bzl", "esp32_14_2_0")
load("//:fragments/esp_idf/xtensa_esp_elf_14.2.0/esp32s3.bzl", "esp32s3_14_2_0")

esp32 = {
    "esp32_gcc_14.2.0": {
        "esp32-xtensa_lx6": esp32_14_2_0,
    },
}

esp32s3 = {
    "esp32s3_gcc_14.2.0": {
        "esp32s3-xtensa_lx7": esp32s3_14_2_0,
    },
}
