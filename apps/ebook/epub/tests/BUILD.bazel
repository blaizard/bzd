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
        "//apps/ebook:providers",
        "//apps/ebook/epub",
        "//apps/ebook/epub:metadata",
    ],
)

py_library(
    name = "driver_mock",
    srcs = [
        "driver_mock.py",
    ],
    visibility = ["//apps/ebook:__subpackages__"],
    deps = [
        "//apps/ebook/epub:metadata",
    ],
)
