"""Metadata for ESP32 toolchains."""

load("//toolchains/esp_idf:xtensa_esp_elf_14.2.0/esp32.bzl", "esp32_14_2_0")
load("//toolchains/esp_idf:xtensa_esp_elf_14.2.0/esp32s3.bzl", "esp32s3_14_2_0")

esp32 = {
    "esp32_gcc_14.2.0": {
        "esp32_idf-xtensa_lx6": esp32_14_2_0,
    },
}

esp32s3 = {
    "esp32s3_gcc_14.2.0": {
        "esp32s3_idf-xtensa_lx7": esp32s3_14_2_0,
    },
}
