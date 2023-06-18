load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_doc_pip//:requirements.bzl", "requirement")

py_binary(
    name = "builder",
    srcs = [
        "builder.py",
    ],
    data = [
        ":mkdocs.yml.template",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "mkdocs_wrapper",
    srcs = [
        "mkdocs_wrapper.py",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        requirement("mkdocs"),
        requirement("mkdocs-material"),
        requirement("mkdocs-exclude"),
    ],
)

py_binary(
    name = "preprocessor",
    srcs = ["preprocessor.py"],
    visibility = ["//visibility:public"],
)
