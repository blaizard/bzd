load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "parser",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:dict",
        "@bzd_python//bzd/utils:result",
    ],
)

py_binary(
    name = "playground",
    srcs = [
        "playground.py",
    ],
    args = [
        "python/bzd/parser/scratch_pad.py",
    ],
    deps = [
        ":parser",
        "@bzd_python//bzd/utils:file",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("Parser", "README.md"),
    ],
    visibility = ["//python/bzd:__pkg__"],
)
