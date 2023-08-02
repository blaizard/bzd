load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:defs.bzl", "py_library")

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
