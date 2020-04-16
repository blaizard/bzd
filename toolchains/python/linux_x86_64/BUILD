load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")

py_runtime_pair(
    name = "py_runtime_pair",
    py3_runtime = "@python3_linux_x86_64//:runtime",
    visibility = ["//visibility:public"],
)

toolchain(
    name = "toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":py_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
)
