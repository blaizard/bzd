load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@stardoc//stardoc:stardoc.bzl", "stardoc")

_RULE_NAMES = [
    "bdl",
    "cc",
    "docker",
]

[stardoc(
    name = name,
    out = "{}.md".format(name),
    format = "markdown",
    input = "//tools/bazel_build/rules:{}.bzl".format(name),
    deps = [
        "//tools/bazel_build/rules:{}".format(name),
    ],
) for name in _RULE_NAMES]

doc_library(
    name = "bazel",
    srcs = [
        "{}.md".format(name)
        for name in _RULE_NAMES
    ],
    visibility = ["//docs:__pkg__"],
)
