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
        "entities/**/*.py",
        "visitors/**/*.py",
    ]) + [
        "visitor.py",
        "object.py",
        "builtins.py",
    ],
    visibility = [":__subpackages__"],
    deps = [
        "//python/bzd/parser",
        "//python/bzd/utils:memoized_property",
        "//python/bzd/validation",
    ],
)

py_library(
    name = "generators",
    srcs = glob([
        "generators/**/*.py",
    ]),
    data = glob([
        "generators/**/*.btl",
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
        ":generators",
        ":grammar",
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
