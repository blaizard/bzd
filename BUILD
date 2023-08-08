load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

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
        "@bzd_sanitizer//actions/structure",
        "--use",
        "@bzd_sanitizer//actions/buildifier",
        "--use",
        "@bzd_sanitizer//actions/json",
        "--use",
        "//tools/nodejs:clang_format",
        "--use",
        "//tools/cc:clang_format",
        "--use",
        "//tools/python:yapf",
        "--use",
        "//tools/ci:generate",
        "--use",
        "//tools/shell:generate",
    ],
    binary = "@bzd_sanitizer//:sanitizer",
)
