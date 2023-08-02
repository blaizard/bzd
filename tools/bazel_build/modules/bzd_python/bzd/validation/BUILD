load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "validation",
    srcs = glob([
        "*.py",
        "constraints/**/*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
        "//python/bzd/utils:decorators",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("Validation", "README.md"),
    ],
    visibility = ["//python/bzd:__pkg__"],
)
