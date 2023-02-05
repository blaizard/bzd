load("//tools/bazel_build/rules:documentation.bzl", "doc_library")

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
