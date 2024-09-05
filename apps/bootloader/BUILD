load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    include_workspace_status = ["STABLE_VERSION"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "bootloader",
    srcs = [
        "binary.py",
        "bootloader.py",
        "manifest.py",
        "mutex.py",
        "singleton.py",
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
        "//apps/artifacts/api/python/release",
        "//apps/artifacts/api/python/release:mock",
        "@bzd_python//bzd/utils:logging",
        "@bzd_python//bzd/utils:run",
        "@bzd_python//bzd/utils:scheduler",
    ],
)
