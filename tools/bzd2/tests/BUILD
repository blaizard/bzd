load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "grammar",
    srcs = [
        "grammar.py",
    ],
    args = [
        "$(location grammar.bdl)",
    ],
    data = [
        "grammar.bdl",
    ],
    deps = [
        "//tools/bzd2:grammar",
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
        "//tools/bzd2:format",
        "//tools/bzd2:grammar",
    ],
)
