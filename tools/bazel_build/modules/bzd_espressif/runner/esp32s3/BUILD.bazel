load("@bzd_lib//:defs.bzl", "bzd_make_runner_toolchain")
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
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
        "@esptool//:esptool": "esptool",
        "@qemu//:qemu-system-xtensa": "qemu",
    },
)

bzd_make_runner_toolchain(
    name = "qemu_toolchain",
    runner = ":qemu",
    target_compatible_with = [
        "@bzd_platforms//al:esp32s3",
        "@bzd_platforms//isa:xtensa_lx7",
    ],
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
        "//tools/esptool:executor_uart": "executor_uart",
        "@esp32s3_xtensa_lx7_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32s3_xtensa_lx7_sdk//:bin/partition-table.bin": "partitions",
        "@esptool//:esptool": "esptool",
    },
)

bzd_make_runner_toolchain(
    name = "uart_toolchain",
    runner = ":uart",
    target_compatible_with = [
        "@bzd_platforms//al:esp32s3",
        "@bzd_platforms//isa:xtensa_lx7",
    ],
)
