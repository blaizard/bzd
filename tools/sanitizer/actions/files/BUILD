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
        "//tools/sanitizer/utils/python:workspace",
        "@bzd_bdl//bdl:lib",
        "@bzd_python//bzd/utils:worker",
    ],
)
