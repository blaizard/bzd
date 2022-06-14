load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "build",
    srcs = [
        "build.py",
    ],
    deps = [
        "//python/bzd/command_extractor:gcc",
        "//python/bzd/utils:run",
    ],
)
