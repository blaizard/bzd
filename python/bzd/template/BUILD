load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "template",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/parser",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("Template", "README.md"),
    ],
    visibility = ["@bzd_python//:__pkg__"],
)
