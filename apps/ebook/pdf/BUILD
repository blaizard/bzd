load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "pdf_to_images",
    srcs = [
        "pdf_to_images.py",
    ],
    visibility = ["//apps/ebook:__pkg__"],
    deps = [
        "//apps/ebook:flow",
        "//apps/ebook:providers",
        "//apps/ebook:utils",
        requirement("pymupdf"),
    ],
)
