load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "json",
    srcs = [
        "cli.py",
    ],
    main = "cli.py",
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd_sanitizer:worker",
    ],
)
