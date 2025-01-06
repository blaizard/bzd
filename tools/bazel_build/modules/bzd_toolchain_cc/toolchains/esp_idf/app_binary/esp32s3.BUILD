load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "app_build_to_image",
    locations = {
        "@esptool//:esptool": "esptool",
    },
    command = """
        {esptool} --chip ESP32-S3 elf2image -o "$2" "$1" > /dev/null
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "app_executor_qemu",
    locations = {
        "@esp32_qemu//:qemu-system-xtensa": "qemu",
        "@bzd_python//bzd/utils:binary_builder": "builder",
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
    },
    command = """
        {builder} --size 4MB --chunk {bootloader},0x0 --chunk {partitions},0x8000 --chunk $2,0x10000 qemu_flash.bin
        {qemu} -machine esp32s3 -m 4M -drive file=qemu_flash.bin,if=mtd,format=raw -no-reboot -nographic
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "app_executor_uart",
    locations = {
        "@bzd_toolchain_cc//toolchains/esp_idf/esptool:executor_uart": "executor_uart",
        "@esptool//:esptool": "esptool",
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
    },
    command = """
        {executor_uart} \
                --esptool {esptool} \
                --device_vid_pid 0x303A 0x1001 \
                --size 4MB --chunk {bootloader},0x0 --chunk {partitions},0x8000 --chunk $2,0x10000 \
                -- \
                --chip esp32s3 --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB
    """,
    visibility = ["//visibility:public"],
)
