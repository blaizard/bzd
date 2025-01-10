load("@bzd_lib//:executable_test.bzl", "bzd_executable_test")
load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@rules_python//python:defs.bzl", "py_binary", "py_test")
load("//apps/bootloader:defs.bzl", "bzd_bootloader_binary")

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

py_binary(
    name = "square",
    srcs = [
        "square.py",
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

py_binary(
    name = "update",
    testonly = True,
    srcs = ["update.py"],
    deps = [
        "//apps/bootloader",
    ],
)

bzd_executable_test(
    name = "update_test",
    executable = ":update",
    expected_output = "SUCCESS!",
    expected_returncode = "0",
)

bzd_executable_test(
    name = "noop_test",
    executable = "//apps/bootloader",
    expected_output = "Running",
    expected_returncode = "0",
)

bzd_config(
    name = "bootloader_config",
    data = [
        "//apps/bootloader/tests:square",
    ],
    values = {
        "application": "apps/bootloader/tests/square",
    },
)

bzd_bootloader_binary(
    name = "with_application",
    configs = {
        "//apps/bootloader:config": ":bootloader_config",
    },
)

bzd_executable_test(
    name = "application_test",
    args = ["12"],
    executable = ":with_application",
    expected_output = "144",
    expected_returncode = "0",
)
