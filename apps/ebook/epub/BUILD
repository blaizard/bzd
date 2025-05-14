load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "epub",
    srcs = [
        "epub.py",
    ],
    visibility = ["//apps/ebook:__subpackages__"],
    deps = [
        ":driver_selenium",
        ":metadata",
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ],
)

py_library(
    name = "metadata",
    srcs = [
        "metadata.py",
    ],
    visibility = ["//apps/ebook/epub:__subpackages__"],
    deps = [
        "//apps/ebook:providers",
    ],
)

py_library(
    name = "driver_selenium",
    srcs = [
        "driver_selenium.py",
    ],
    deps = [
        ":metadata",
    ] + requirement("selenium") + requirement("webdriver-manager") + requirement("pillow"),
)
