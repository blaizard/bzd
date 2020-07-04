load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "buildifier",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--buildifier",
        "$(location @buildifier//file)",
    ],
    data = [
        "@buildifier//file",
    ],
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//python/bzd/utils:run",
    ],
)
