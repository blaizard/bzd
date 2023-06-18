load("@rules_doc:defs.bzl", "doc_binary", "doc_library")

doc_library(
    name = "lib1",
    srcs = [
        ("Doc1", "doc1.md"),
        "doc2.md",
    ],
)

doc_library(
    name = "lib2",
    srcs = [
        ("Doc3", "doc3.md"),
        "doc4.md",
    ],
)

doc_binary(
    name = "tests",
    srcs = [
        ("Library", ":lib1"),
        ":lib2",
    ],
)
