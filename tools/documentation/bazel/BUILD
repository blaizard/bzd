load("@io_bazel_stardoc//stardoc:stardoc.bzl", "stardoc")
load("//tools/bazel.build:copy_outputs.bzl", "copy_outputs")

rule_names = [
    "nodejs",
    "nodejs_web",
    "package",
]

[stardoc(
    name = name,
    out = "{}.md".format(name),
    format = "markdown",
    input = "//tools/bazel.build/rules:{}.bzl".format(name),
    deps = [
        "//tools/bazel.build/rules:{}".format(name),
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
