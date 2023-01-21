load("@io_bazel_stardoc//stardoc:stardoc.bzl", "stardoc")
load("//tools/bazel_build:copy_outputs.bzl", "copy_outputs")

rule_names = [
    "bdl",
    "docker",
    "nodejs",
    "nodejs_web",
    "package",
    "cc",
]

[stardoc(
    name = name,
    out = "{}.md".format(name),
    format = "markdown",
    input = "//tools/bazel_build/rules:{}.bzl".format(name),
    deps = [
        "//tools/bazel_build/rules:{}".format(name),
    ],
) for name in rule_names]

copy_outputs(
    name = "bazel",
    inputs = [
        ":nodejs",
        ":nodejs_web",
        ":package",
    ],
)
