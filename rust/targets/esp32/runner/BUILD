load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "qemu",
    command = """
        {esptool} --chip esp32 elf2image -o "./image" "$1"
        {builder} --size 4MB --chunk {bootloader},0x1000 --chunk {partitions},0x8000 --chunk ./image,0x10000 ./qemu_flash.bin
        {qemu} --binary "$1" -- -machine esp32 -m 4M -drive file=./qemu_flash.bin,if=mtd,format=raw -no-reboot -nographic
    """,
    locations = {
        "@bzd_python//bzd/utils:binary_builder": "builder",
        "@esp32_qemu//:qemu-system-xtensa": "qemu",
        "@esp32_xtensa_lx6_sdk//:bin/bootloader.bin": "bootloader",
        "@esp32_xtensa_lx6_sdk//:bin/partition-table.bin": "partitions",
        "@esptool//:esptool": "esptool",
    },
)
