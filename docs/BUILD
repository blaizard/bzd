load("//tools/bazel_build/rules:documentation.bzl", "doc_binary")

doc_binary(
    name = "docs",
    srcs = [
        ("Home", "//:README.md"),
        ("BDL", "//docs/bdl"),
        ("Bazel", "//docs/api/bazel"),
        ("C++", "//cc:documentation"),
    ],
    data = glob([
        "**/*.jpg",
        "**/*.png",
    ]),
)
