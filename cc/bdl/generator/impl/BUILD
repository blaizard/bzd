load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "visitor",
    srcs = glob([
        "**/*.py",
    ]),
    data = glob([
        "template/*.btl",
    ]),
    visibility = [
        "//cc/bdl/generator:__subpackages__",
    ],
    deps = [
        "@bzd_bdl//bdl:visitor",
        "@bzd_python//bzd/parser",
        "@bzd_python//bzd/template",
    ],
)
