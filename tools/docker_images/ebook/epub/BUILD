load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "epub",
    srcs = [
        "epub.py",
    ],
    visibility = ["//tools/docker_images/ebook:__subpackages__"],
    deps = [
        ":driver_selenium",
        ":metadata",
        "//tools/docker_images/ebook:flow",
        "//tools/docker_images/ebook:providers",
    ],
)

py_library(
    name = "metadata",
    srcs = [
        "metadata.py",
    ],
    visibility = ["//tools/docker_images/ebook/epub:__subpackages__"],
)

py_library(
    name = "driver_selenium",
    srcs = [
        "driver_selenium.py",
    ],
    deps = [
        ":metadata",
    ] + requirement("selenium") + requirement("webdriver-manager"),
)
