load("@rules_python//python:defs.bzl", "py_library")

exports_files([
    "executor_qemu.py",
    "executor_uart.py",
])

filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
        "qemu/.gdbinit",
    ]),
)

py_library(
    name = "targets",
    srcs = ["targets.py"],
    data = [
        ":esp32_bin",
    ],
    visibility = ["//visibility:public"],
)
