load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:defs.bzl", "py_library")

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

doc_library(
    name = "documentation",
    srcs = [
        ("Parser", "README.md"),
    ],
    visibility = ["@bzd_python//:__pkg__"],
)
