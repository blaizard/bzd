load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_extern_binary")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_nodejs_extern_binary(
    name = "prettier_binary",
    binary = "prettier/bin/prettier.cjs",
    packages = [
        "@bzd_sanitizer_nodejs_deps//:prettier",
    ],
    visibility = ["//actions:__subpackages__"],
)

py_binary(
    name = "cli",
    srcs = [
        "cli.py",
    ],
    deps = [
        "//python/bzd_sanitizer:worker",
        "@bzd_python//bzd/utils:run",
    ],
)

sh_binary_wrapper(
    name = "prettier",
    command = "{binary} --prettier {prettier} $@",
    locations = {
        ":cli": "binary",
        ":prettier_binary": "prettier",
    },
    visibility = ["//visibility:public"],
)
