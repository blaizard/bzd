load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_test(
    name = "metadata",
    srcs = ["metadata_test.py"],
    data = [
        ":data/sample1.epub",
    ],
    main = "metadata_test.py",
    deps = [
        ":driver_mock",
        "//tools/docker_images/ebook:providers",
        "//tools/docker_images/ebook/epub",
        "//tools/docker_images/ebook/epub:metadata",
    ],
)

py_library(
    name = "driver_mock",
    srcs = [
        "driver_mock.py",
    ],
    visibility = ["//tools/docker_images/ebook:__subpackages__"],
    deps = [
        "//tools/docker_images/ebook/epub:metadata",
    ],
)
