load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "calibre",
    srcs = [
        "calibre.py",
    ],
    deps = [
        "//tools/docker_images/ebook:flow",
    ],
)

py_library(
    name = "remove_drm",
    srcs = [
        "remove_drm.py",
    ],
    visibility = ["//tools/docker_images/ebook:__pkg__"],
    deps = [
        ":calibre",
        "//tools/docker_images/ebook:flow",
        "//tools/docker_images/ebook:providers",
    ],
)
