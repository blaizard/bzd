load("@rules_python//python:defs.bzl", "py_test")
load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")

py_runtime_pair(
    name = "py_runtime_pair",
    py3_runtime = "@python3_linux_x86_64//:runtime",
    visibility = ["//visibility:public"],
)

toolchain(
    name = "toolchain",
    # Only exec is needed as this is not a cross platform compilation toolchain
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":py_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
)

py_test(
    name = "test",
    srcs = ["python_test.py"],
    main = "python_test.py",
)
