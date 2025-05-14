load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "images_to_pdf",
    srcs = [
        "images_to_pdf.py",
    ],
    visibility = ["//apps/ebook:__pkg__"],
    deps = [
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ] + requirement("pillow"),
)
