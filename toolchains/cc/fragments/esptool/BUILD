load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
        "qemu/.gdbinit",
    ]),
)

sh_binary_wrapper(
    name = "esp32_metadata",
    binary = "//tools/bazel_build/rules/assets/cc:profiler",
    command = """
        {binary} --groups $(location profile_groups.json) "$@"
    """,
    data = [
        "profile_groups.json",
    ],
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "esp32_build",
    binary = "@esptool//:esptool",
    command = """
        {binary} --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1"
    """,
    visibility = ["//visibility:public"],
)

py_binary(
    name = "executor_uart",
    srcs = [
        "executor_uart.py",
    ],
    data = [
        "@esptool",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":targets",
        "//python/bzd:env",
        "//python/bzd/utils:run",
        "//python/bzd/utils:uart",
    ],
)

py_binary(
    name = "executor_qemu",
    srcs = ["executor_qemu.py"],
    visibility = ["//visibility:public"],
    deps = [
        ":targets",
        "//python/bzd/utils:run",
    ],
)

py_library(
    name = "targets",
    srcs = ["targets.py"],
    data = [
        ":esp32_bin",
    ],
)
