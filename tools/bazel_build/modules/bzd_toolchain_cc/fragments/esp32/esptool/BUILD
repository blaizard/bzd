load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "executor_uart",
    srcs = [
        ":executor_uart.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/logging",
        "@bzd_python//bzd/utils:binary_builder",
        "@bzd_python//bzd/utils:run",
        "@bzd_python//bzd/utils:uart",
    ],
)

sh_binary_wrapper(
    name = "esp32_metadata",
    command = """
        {profiler} --groups {profile_groups} "$@"
    """,
    locations = {
        "@bzd_toolchain_cc//binary/elf:profiler": "profiler",
        "@bzd_toolchain_cc//fragments/esp32/esptool:profile_groups.json": "profile_groups",
    },
    visibility = ["//visibility:public"],
)
