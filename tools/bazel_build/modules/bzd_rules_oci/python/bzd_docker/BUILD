load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "image",
    srcs = ["image.py"],
    data = [
        "@bzd_rules_oci//:config.json",
    ],
    imports = [".."],
    visibility = ["//visibility:public"],
)
