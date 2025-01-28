load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")
load(":defs.bzl", "rustfmt_binary")

py_binary(
    name = "cli",
    srcs = [
        "cli.py",
    ],
    main = "cli.py",
    deps = [
        "//python/bzd_sanitizer:worker",
        "@bzd_python//bzd/utils:run",
    ],
)

rustfmt_binary(
    name = "rustfmt_binary",
)

sh_binary_wrapper(
    name = "rustfmt",
    command = "{cli} --rustfmt {rustfmt} $@",
    locations = {
        ":cli": "cli",
        ":rustfmt_binary": "rustfmt",
    },
    visibility = ["//visibility:public"],
)
