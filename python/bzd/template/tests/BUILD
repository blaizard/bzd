load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "template",
    srcs = [
        "template.py",
    ],
    data = [
        "nested_template.txt",
    ],
    deps = [
        "//python/bzd/template",
    ],
)
