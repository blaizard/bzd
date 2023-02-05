load("//tools/bazel_build/rules:documentation.bzl", "doc_binary")

doc_binary(
    name = "docs",
    srcs = [
        ("**", "//:documentation"),
        ("BDL", "//docs/bdl"),
        ("Bazel", "//docs/api/bazel"),
        ("C++", "//cc:documentation"),
        ("Python", "//python/bzd:documentation"),
    ],
    data = glob([
        "**/*.jpg",
        "**/*.png",
    ]),
)
