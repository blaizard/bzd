load("@bzd_sanitizer_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "yapf",
    srcs = [
        "cli.py",
    ],
    main = "cli.py",
    visibility = ["//visibility:public"],
    deps = [
        requirement("yapf"),
        "//python/bzd_sanitizer:worker",
    ],
)
