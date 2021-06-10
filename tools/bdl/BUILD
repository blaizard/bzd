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
    srcs = glob([
        "entity/**/*.py",
        "visitors/*.py",
    ]) + [
        "visitor.py",
        "result.py",
    ],
    visibility = [":__subpackages__"],
    deps = [
        "//python/bzd/parser",
    ],
)

py_library(
    name = "lang",
    srcs = glob([
        "lang/**/*.py",
    ]),
    data = glob([
        "lang/**/*.template",
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
        ":grammar",
        ":lang",
        ":visitor",
    ],
)

py_binary(
    name = "bdl",
    srcs = [
        "main.py",
    ],
    main = "main.py",
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
