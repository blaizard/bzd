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
        "//tools/docker_images/ebook/calibre:remove_drm",
        "//tools/docker_images/ebook/cbz",
        "//tools/docker_images/ebook/epub",
        "//tools/docker_images/ebook/pillow:images_to_pdf",
    ],
)

py_library(
    name = "providers",
    srcs = [
        "providers.py",
    ],
    visibility = [
        "//tools/docker_images/ebook:__subpackages__",
    ],
)

py_library(
    name = "flow",
    srcs = [
        "flow.py",
    ],
    visibility = [
        "//tools/docker_images/ebook:__subpackages__",
    ],
    deps = [
        ":providers",
    ],
)

bzd_python_oci(
    name = "image",
    binary = ":ebook",
)
