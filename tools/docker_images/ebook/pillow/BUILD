load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "images_to_pdf",
    srcs = [
        "images_to_pdf.py",
    ],
    visibility = ["//tools/docker_images/ebook:__pkg__"],
    deps = [
        "//tools/docker_images/ebook:flow",
        "//tools/docker_images/ebook:providers",
    ] + requirement("pillow"),
)
