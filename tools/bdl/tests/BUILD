load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "grammar",
    srcs = [
        "grammar.py",
    ],
    deps = [
        "//tools/bdl:grammar",
    ],
)

py_test(
    name = "formatter",
    srcs = [
        "formatter.py",
    ],
    args = [
        "$(location formatter.bdl.txt)",
    ],
    data = [
        "formatter.bdl.txt",
    ],
    deps = [
        "//tools/bdl:grammar",
        "//tools/bdl:lang",
    ],
)
