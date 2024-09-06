load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_python//python:pip.bzl", "compile_pip_requirements")

doc_library(
    name = "documentation",
    srcs = [
        ("**", "@bzd_python//bzd/parser:documentation"),
        ("**", "@bzd_python//bzd/template:documentation"),
        ("**", "@bzd_python//bzd/validation:documentation"),
    ],
    visibility = ["//visibility:public"],
)

compile_pip_requirements(
    name = "requirements",
    timeout = "long",
    generate_hashes = False,
    requirements_in = ":requirements.in",
    requirements_txt = ":requirements.txt",
)
