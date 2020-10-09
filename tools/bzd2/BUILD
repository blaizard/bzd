load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "parser",
    srcs = glob([
        "parser.py",
        "fragments.py",
        "element.py",
        "grammar.py",
    ]),
    visibility = [
        "//tools/bzd2/tests:__pkg__",
    ],
)

py_binary(
    name = "bzd2",
    srcs = [
        "main.py",
    ],
    main = "main.py",
    deps = [
        ":parser",
    ],
)
