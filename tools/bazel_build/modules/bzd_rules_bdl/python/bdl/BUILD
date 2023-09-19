load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "grammar",
    srcs = [
        "grammar.py",
    ],
    imports = [".."],
    visibility = [":__subpackages__"],
    deps = [
        "@bzd_python//bzd/parser",
    ],
)

py_library(
    name = "visitor",
    srcs = glob([
        "contracts/**/*.py",
        "entities/**/*.py",
        "visitors/**/*.py",
    ]) + [
        "builtins.py",
        "object.py",
        "visitor.py",
    ],
    imports = [".."],
    visibility = [":__subpackages__"],
    deps = [
        "@bzd_python//bzd/parser",
        "@bzd_python//bzd/utils:decorators",
        "@bzd_python//bzd/validation",
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
    imports = [".."],
    visibility = [":__subpackages__"],
    deps = [
        ":visitor",
        "@bzd_python//bzd/template",
    ],
)

py_library(
    name = "lib",
    srcs = [
        "lib.py",
    ],
    imports = [".."],
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
    imports = [".."],
    main = "main.py",
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
