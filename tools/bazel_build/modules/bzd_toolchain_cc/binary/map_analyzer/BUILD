load("@bzd_toolchain_cc_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "map_analyzer",
    srcs = ["map_analyzer.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyelftools"),
        "@bzd_toolchain_cc//binary/map_analyzer/parser",
    ],
)
