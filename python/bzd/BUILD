load("@rules_python//python:defs.bzl", "py_library")
load("//tools/bazel_build/rules:documentation.bzl", "doc_library")

py_library(
    name = "env",
    srcs = ["env.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("**", "//python/bzd/parser:documentation"),
        ("**", "//python/bzd/template:documentation"),
        ("**", "//python/bzd/validation:documentation"),
    ],
    visibility = ["//docs:__pkg__"],
)
