load("@rules_python//python:defs.bzl", "py_binary")

exports_files([
    "robots.txt",
])

py_binary(
    name = "sitemap",
    srcs = [
        "sitemap.py",
    ],
    visibility = ["//visibility:public"],
)
