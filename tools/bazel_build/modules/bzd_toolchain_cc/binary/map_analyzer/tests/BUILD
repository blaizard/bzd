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
    deps = [
        "@bzd_toolchain_cc//binary/map_analyzer/parser",
    ],
)
