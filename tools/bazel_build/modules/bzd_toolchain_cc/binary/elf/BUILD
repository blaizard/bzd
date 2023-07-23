load("@rules_python//python:defs.bzl", "py_binary")
load("@bzd_toolchain_cc_pip//:requirements.bzl", "requirement")

py_binary(
    name = "profiler",
    srcs = ["profiler.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyelftools"),
    ],
)
