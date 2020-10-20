load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "grammar",
    srcs = [
        "grammar.py",
    ],
    visibility = [":__subpackages__"],
    deps = [
        "//python/bzd/parser",
    ],
)

py_library(
    name = "visitor",
    srcs = [
        "visitor.py",
    ],
    visibility = [":__subpackages__"],
    deps = [
        "//python/bzd/parser",
    ],
)

py_library(
    name = "format",
    srcs = glob([
        "format/*.py",
    ]),
    data = glob([
        "format/**/*.template",
    ]),
    visibility = [":__subpackages__"],
    deps = [
        ":visitor",
        "//python/bzd/template",
    ],
)

py_library(
    name = "lib",
    srcs = [
        "lib.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":format",
        ":grammar",
        ":visitor",
    ],
)

py_binary(
    name = "bzd2",
    srcs = [
        "main.py",
    ],
    main = "main.py",
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
