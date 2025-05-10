load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "convert_images",
    srcs = [
        "convert_images.py",
    ],
    visibility = ["//tools/docker_images/ebook:__pkg__"],
    deps = [
        "//tools/docker_images/ebook:flow",
        "//tools/docker_images/ebook:providers",
    ],
)
