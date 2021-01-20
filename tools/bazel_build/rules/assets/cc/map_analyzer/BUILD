load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "map_analyzer",
    srcs = ["map_analyzer.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//tools/bazel_build/rules/assets/cc/map_analyzer/parser",
    ],
)
