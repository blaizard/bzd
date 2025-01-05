"""ESP32S3 definition."""

load("//:fragments/esp_idf/xtensa_esp_elf_14.2.0/linux_x86_64/esp32s3.bzl", "esp32s3_linux_x86_64")

esp32s3_14_2_0 = {
    "linux-x86_64": esp32s3_linux_x86_64,
}
