load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "structure",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//tools/sanitizer/utils:workspace",
        "//tools/sanitizer/utils:filter",
    ],
)
