"""ESP32 definition."""

load("//:fragments/esp_idf/xtensa_esp_elf_14.2.0/linux_x86_64/esp32.bzl", "esp32_linux_x86_64")

esp32_14_2_0 = {
    "linux-x86_64": esp32_linux_x86_64,
}
