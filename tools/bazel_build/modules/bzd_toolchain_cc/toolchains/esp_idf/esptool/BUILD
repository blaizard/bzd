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
    name = "metadata",
    command = """
        {profiler} --groups {profile_groups} "$@"
    """,
    locations = {
        "//binary/elf:profiler": "profiler",
        "//toolchains/esp_idf/esptool:profile_groups.json": "profile_groups",
    },
    visibility = ["//visibility:public"],
)
