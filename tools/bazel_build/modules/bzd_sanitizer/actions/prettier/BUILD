load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_nodejs_binary(
    name = "prettier_wrapper",
    srcs = [
        "prettier_wrapper.mjs",
    ],
    main = "prettier_wrapper.mjs",
    packages = {
        "prettier": "",
    },
    visibility = ["@bzd_sanitizer//actions:__subpackages__"],
)

py_binary(
    name = "cli",
    srcs = [
        "cli.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
        "@bzd_sanitizer//lib:worker",
    ],
)

sh_binary_wrapper(
    name = "prettier",
    command = "{binary} --prettier {prettier} $@",
    locations = {
        ":cli": "binary",
        ":prettier_wrapper": "prettier",
    },
    visibility = ["//visibility:public"],
)
