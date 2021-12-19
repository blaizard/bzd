load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
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
    name = "esp32_prepare",
    binary = "@esptool//:esptool",
    command = """
        {binary} --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "2MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1"
    """,
    visibility = ["//visibility:public"],
)

py_binary(
    name = "execute",
    srcs = [
        "execute.py",
    ],
    data = [
        ":esp32_bin",
        "@esptool",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
        "//python/bzd/utils:terminal",
    ],
)
