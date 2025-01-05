load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "app_build_to_image",
    locations = {
        "@esptool//:esptool": "esptool",
    },
    command = """
        {esptool} --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1" > /dev/null
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "app_executor_qemu",
    locations = {
        "@esp32_qemu//:qemu": "qemu",
        "@bzd_python//bzd/utils:binary_builder": "builder",
        "@esp32_xtensa_lx6_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32_xtensa_lx6_sdk//:bin/partition-table.bin": "partitions",
    },
    command = """
        {builder} --size 4MB --chunk {bootloader},0x1000 --chunk {partitions},0x8000 --chunk $2,0x10000 qemu_flash.bin
        {qemu} -no-reboot -nographic -machine esp32 -m 4 -drive file=qemu_flash.bin,if=mtd,format=raw
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "app_executor_uart",
    locations = {
        "@bzd_toolchain_cc//fragments/esp_idf/esptool:executor_uart": "executor_uart",
        "@esptool//:esptool": "esptool",
        "@esp32_xtensa_lx6_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32_xtensa_lx6_sdk//:bin/partition-table.bin": "partitions",
    },
    command = """
        {executor_uart} \
                --esptool {esptool} \
                --device_vid_pid 0x10C4 0xEA60 \
                --size 4MB --chunk {bootloader},0x1000 --chunk {partitions},0x8000 --chunk $2,0x10000 \
                -- \
                --chip esp32 --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect
    """,
    visibility = ["//visibility:public"],
)
