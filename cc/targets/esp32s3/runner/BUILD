load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "qemu",
    command = """
        {esptool} --chip esp32-s3 elf2image -o "./image" "$1"
        {builder} --size 4MB --chunk {bootloader},0x0 --chunk {partitions},0x8000 --chunk ./image,0x10000 ./qemu_flash.bin
        {qemu} --binary "$1" -- -machine esp32s3 -m 4M -drive file=./qemu_flash.bin,if=mtd,format=raw -no-reboot -nographic
    """,
    locations = {
        "@bzd_python//bzd/utils:binary_builder": "builder",
        "@esp32_qemu//:qemu-system-xtensa": "qemu",
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
        "@esptool//:esptool": "esptool",
    },
)

sh_binary_wrapper(
    name = "uart",
    command = """
        {esptool} --chip esp32-s3 elf2image -o "./image" "$1"
        {executor_uart} \
                --esptool {esptool} \
                --device_vid_pid 0x303A 0x1001 \
                --size 4MB --chunk {bootloader},0x0 --chunk {partitions},0x8000 --chunk ./image,0x10000 \
                -- \
                --chip esp32s3 --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB
    """,
    locations = {
        "@bzd_toolchain_cc//toolchains/esp_idf/esptool:executor_uart": "executor_uart",
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
        "@esptool//:esptool": "esptool",
    },
)
