load("@bzd_rules_doc//doc:defs.bzl", "doc_library")

doc_library(
    name = "docs",
    srcs = [
        ("Overview", "overview.md"),
        ("Syntax", "syntax.md"),
        ("Components", "components.md"),
        ("Build", "build.md"),
        ("Generators", ":generators"),
        ("Executors", "executors.md"),
        ("Gateway", "gateway.md"),
        ("System", "system.md"),
    ],
    visibility = ["//visibility:public"],
)

doc_library(
    name = "generators",
    srcs = [
        ("C++", "cc.md"),
    ],
)
