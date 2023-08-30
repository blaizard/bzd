load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@bzd_rules_doc//doc:defs.bzl", "doc_library")

exports_files([
    ".clang-tidy",
])

doc_library(
    name = "documentation",
    srcs = [
        ("Home", "README.md"),
    ],
    visibility = ["//docs:__pkg__"],
)

sh_binary_wrapper(
    name = "sanitizer",
    args = [
        "--use",
        "@bzd_sanitizer//actions/markdown",
        "--use",
        "@bzd_sanitizer//actions/yaml",
        "--use",
        "@bzd_sanitizer//actions/json",
        "--use",
        "@bzd_sanitizer//actions/structure",
        "--use",
        "@bzd_sanitizer//actions/buildifier",
        "--use",
        "//tools/cc:clang_format",
        "--use",
        "//tools/nodejs:prettier",
        "--use",
        "//tools/python:yapf",
        "--use",
        "//tools/ci:generate",
        "--use",
        "//tools/shell:generate",
    ],
    binary = "@bzd_sanitizer//:sanitizer",
)
