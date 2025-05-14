load("@bzd_python//:defs.bzl", "bzd_python_oci")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "ebook",
    srcs = [
        "ebook.py",
    ],
    deps = [
        ":flow",
        ":providers",
        "//apps/ebook/calibre:remove_drm",
        "//apps/ebook/comics:cbr",
        "//apps/ebook/comics:cbz",
        "//apps/ebook/epub",
        "//apps/ebook/pillow:images_to_pdf",
    ],
)

py_library(
    name = "providers",
    srcs = [
        "providers.py",
    ],
    visibility = [
        "//apps/ebook:__subpackages__",
    ],
)

py_library(
    name = "flow",
    srcs = [
        "flow.py",
    ],
    visibility = [
        "//apps/ebook:__subpackages__",
    ],
    deps = [
        ":providers",
    ],
)

bzd_python_oci(
    name = "image",
    base = "@oci_ebook",
    binary = ":ebook",
)
