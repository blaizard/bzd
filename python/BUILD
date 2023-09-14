load("@bzd_rules_doc//doc:defs.bzl", "doc_library")

doc_library(
    name = "documentation",
    srcs = [
        ("**", "@bzd_python//bzd/parser:documentation"),
        ("**", "@bzd_python//bzd/template:documentation"),
        ("**", "@bzd_python//bzd/validation:documentation"),
    ],
    visibility = ["//visibility:public"],
)
