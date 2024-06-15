load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@stardoc//stardoc:stardoc.bzl", "stardoc")

stardoc(
    name = "bdl",
    out = "bdl.md",
    format = "markdown",
    input = "@bzd_bdl//:defs.bzl",
    deps = [
        "@bzd_bdl//:defs",
    ],
)

stardoc(
    name = "cc",
    out = "cc.md",
    format = "markdown",
    input = "//cc/bdl:cc.bzl",
    deps = [
        "//cc/bdl:cc",
    ],
)

doc_library(
    name = "bazel",
    srcs = [
        "bdl.md",
        "cc.md",
    ],
    visibility = ["//docs:__pkg__"],
)
