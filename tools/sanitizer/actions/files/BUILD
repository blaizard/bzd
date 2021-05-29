load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "files",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//python/bzd/utils:worker",
        "//python/bzd/yaml",
        "//tools/bdl:lib",
        "//tools/sanitizer/utils/python:workspace",
    ],
)
