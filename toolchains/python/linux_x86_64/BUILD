load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")

py_runtime_pair(
    name = "py_runtime_pair",
    py2_runtime = "@python2_linux_x86_64//:runtime",
    py3_runtime = "@python3_linux_x86_64//:runtime",
    visibility = ["//visibility:public"],
)

toolchain(
    name = "toolchain",
    toolchain = ":py_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
    visibility = ["//visibility:public"],
)
