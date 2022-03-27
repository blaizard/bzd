load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "ci",
    srcs = [
        "ci.py",
    ],
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//python/bzd:env",
        "//tools/ci/jenkins:factory",
        "//tools/ci/quality_gate:factory",
    ],
)
