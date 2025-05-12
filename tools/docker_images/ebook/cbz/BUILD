load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "cbz",
    srcs = [
        "cbz.py",
    ],
    visibility = ["//tools/docker_images/ebook:__pkg__"],
    deps = [
        "//tools/docker_images/ebook:flow",
        "//tools/docker_images/ebook:providers",
    ] + requirement("pillow"),
)
