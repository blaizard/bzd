load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "coverage",
    srcs = ["coverage.py"],
    args = [
        "--genhtml",
        "$(location @lcov//:genhtml)",
    ],
    data = [
        "@lcov//:genhtml",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
