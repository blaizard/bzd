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
    name = "formatter",
    srcs = [
        "formatter.py",
    ],
    visibility = [":__subpackages__"],
    deps = [
        "//python/bzd/parser",
    ],
)

py_binary(
    name = "bzd2",
    srcs = [
        "main.py",
    ],
    main = "main.py",
    deps = [
        ":formatter",
        ":grammar",
    ],
)
