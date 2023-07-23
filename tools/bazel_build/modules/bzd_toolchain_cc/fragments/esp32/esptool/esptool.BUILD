load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("@bzd_toolchain_cc_pip//:requirements.bzl", "requirement")
load("@bazel_skylib//lib:selects.bzl", "selects")

py_binary(
    name = "wrapper",
    main = "esptool.py",
    srcs = [
        "esptool.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyserial")
    ],
    data = glob(["esptool/**"]),
)

sh_binary_wrapper(
    name = "esp32_metadata",
    binary = "@bzd_toolchain_cc//binary/elf:profiler",
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
    binary = ":wrapper",
    command = """
        {binary} --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB" --min-rev 0 --elf-sha256-offset 0xb0 -o "$2" "$1" > /dev/null
    """,
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "executor_uart_wrapped",
    locations = {
        ":executor_uart": "binary",
        ":wrapper": "esptool"
    },
    command = """
        {binary} --esptool {esptool} "$@"
    """,
    visibility = ["//visibility:public"],
)

py_binary(
    name = "executor_uart",
    srcs = [
        "@bzd_toolchain_cc//fragments/esp32/esptool:executor_uart.py",
    ],
    deps = [
        "@bzd_toolchain_cc//fragments/esp32/esptool:targets",
        "@bzd_python//bzd/utils:run",
        "@bzd_python//bzd/utils:uart",
        "@bzd_python//bzd/utils:logging",
    ],
)

py_binary(
    name = "executor_qemu",
    srcs = ["@bzd_toolchain_cc//fragments/esp32/esptool:executor_qemu.py"],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_toolchain_cc//fragments/esp32/esptool:targets",
        "@bzd_python//bzd/utils:run",
    ],
)
