load("@bzd_sanitizer_pip//:requirements.bzl", "requirement")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "cli",
    srcs = [
        "cli.py",
    ],
    main = "cli.py",
    deps = [
        "@bzd_python//bzd/utils:run",
        "@bzd_sanitizer//lib:worker",
    ],
)

py_binary(
    name = "black_binary",
    srcs = [
        "black_binary.py",
    ],
    deps = [
        requirement("black"),
        requirement("typing-extensions"),
    ],
)

sh_binary_wrapper(
    name = "black",
    command = "{binary} --black {black} $@",
    locations = {
        ":black_binary": "black",
        ":cli": "binary",
    },
    visibility = ["//visibility:public"],
)
