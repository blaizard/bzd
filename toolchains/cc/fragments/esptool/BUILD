load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
    ]),
)

sh_binary_wrapper(
    name = "metadata",
    binary = "//tools/bazel_build/rules/assets/cc:profiler",
    command = """
        {binary} --groups $(location //toolchains/cc/fragments/esptool:profile_groups.json) "$1"
    """,
    data = [
        "//toolchains/cc/fragments/esptool:profile_groups.json",
    ],
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "esp32_prepare",
    binary = "@esptool//:esptool",
    command = """
        {binary} --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "2MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1"
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "esp32_exec",
    binary = "@esptool//:esptool",
    command = "{binary} --chip esp32 --port /dev/ttyUSB0 --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 toolchains/cc/fragments/esptool/bin/esp32/bootloader.bin 0x10000 \"$1\" 0x8000 toolchains/cc/fragments/esptool/bin/esp32/partitions_singleapp.bin",
    data = [
        ":esp32_bin",
    ],
    visibility = ["//visibility:public"],
)
