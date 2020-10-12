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
        "//python/bzd/parser",
    ],
)
