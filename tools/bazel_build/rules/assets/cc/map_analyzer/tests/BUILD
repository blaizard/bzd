load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "parser",
    srcs = [
        "parser.py",
    ],
    args = [
        "$(location clang.map)",
        "$(location gcc.map)",
        "$(location esp32_xtensa_lx6_gcc.map)",
    ],
    data = [
        "clang.map",
        "esp32_xtensa_lx6_gcc.map",
        "gcc.map",
    ],
    tags = ["manual"],
    deps = [
        "//tools/bazel_build/rules/assets/cc/map_analyzer/parser",
    ],
)
