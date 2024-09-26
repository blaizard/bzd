load("@rules_python//python:defs.bzl", "py_binary", "py_test")

py_binary(
    name = "noop",
    srcs = [
        "noop.py",
    ],
    visibility = ["//apps/bootloader:__pkg__"],
)

py_binary(
    name = "error",
    srcs = [
        "error.py",
    ],
    visibility = ["//apps/bootloader:__pkg__"],
)

py_binary(
    name = "sleep",
    srcs = [
        "sleep.py",
    ],
    visibility = ["//apps/bootloader:__pkg__"],
)

py_test(
    name = "utils_test",
    srcs = ["utils_test.py"],
    deps = [
        "//apps/bootloader:utils",
    ],
)
