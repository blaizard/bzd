load("@rules_python//python:defs.bzl", "py_library")
load("@bzd_rules_doc//doc:defs.bzl", "doc_library")

py_library(
    name = "template",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
        "//python/bzd/parser",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("Template", "README.md"),
    ],
    visibility = ["//python/bzd:__pkg__"],
)
