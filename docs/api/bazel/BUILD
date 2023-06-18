load("@stardoc//stardoc:stardoc.bzl", "stardoc")
load("@rules_doc//doc:defs.bzl", "doc_library")

_RULE_NAMES = [
    "bdl",
    "cc",
    "docker",
    "nodejs",
    "nodejs_web",
    "package",
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
