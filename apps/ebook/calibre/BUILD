load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "calibre",
    srcs = [
        "calibre.py",
    ],
    deps = [
        "//apps/ebook:flow",
    ],
)

py_library(
    name = "remove_drm",
    srcs = [
        "remove_drm.py",
    ],
    visibility = ["//apps/ebook:__pkg__"],
    deps = [
        ":calibre",
        "//apps/ebook:flow",
        "//apps/ebook:providers",
    ],
)
