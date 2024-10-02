load("@bzd_lib//:executable_test.bzl", "bzd_executable_test")
load("@bzd_lib//config:defs.bzl", "bzd_config", "bzd_config_default")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("//apps/bootloader:defs.bzl", "bzd_bootloader_binary")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    include_workspace_status = ["STABLE_VERSION"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "utils",
    srcs = [
        "utils.py",
    ],
    visibility = [":__subpackages__"],
)

py_binary(
    name = "bootloader",
    srcs = [
        "binary.py",
        "bootloader.py",
    ],
    data = [
        ":config",
        "//apps/bootloader/tests:error",
        "//apps/bootloader/tests:noop",
        "//apps/bootloader/tests:sleep",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":config",
        ":utils",
        "//apps/artifacts/api/python/node",
        "//apps/artifacts/api/python/release",
        "//apps/artifacts/api/python/release:mock",
        "@bzd_python//bzd/logging",
        "@bzd_python//bzd/logging/handler:async_buffered",
        "@bzd_python//bzd/logging/handler:in_memory",
        "@bzd_python//bzd/logging/handler:stderr",
        "@bzd_python//bzd/logging/handler:stub",
        "@bzd_python//bzd/utils:run",
        "@bzd_python//bzd/utils:scheduler",
    ],
)

bzd_executable_test(
    name = "noop_test",
    executable = ":bootloader",
    expected_output = "Running",
    expected_returncode = 0,
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
        ":config": ":bootloader_config",
    },
)

bzd_executable_test(
    name = "application_test",
    args = ["12"],
    executable = ":with_application",
    expected_output = "144",
    expected_returncode = 0,
)
