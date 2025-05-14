load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "comics",
    srcs = [
        "comics.py",
    ],
    visibility = ["//apps/ebook:__subpackages__"],
    deps = [
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ] + requirement("pillow"),
)

py_library(
    name = "cbr",
    srcs = [
        "cbr.py",
    ],
    visibility = ["//apps/ebook:__subpackages__"],
    deps = [
        ":comics",
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ] + requirement("rarfile"),
)

py_library(
    name = "cbz",
    srcs = [
        "cbz.py",
    ],
    visibility = ["//apps/ebook:__subpackages__"],
    deps = [
        ":comics",
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ],
)
