load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "generate",
    srcs = ["generate.py"],
    deps = [
        "//tools/scripts:extract_compiler_cmd",
    ],
)
