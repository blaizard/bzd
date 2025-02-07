load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary", "bzd_nodejs_web_library")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_nodejs_binary(
    name = "diagram",
    srcs = [
        "bdl_to_elk.mjs",
        "diagram.mjs",
    ],
    main = "diagram.mjs",
    packages = [
        "@bzd_bdl_nodejs_deps//:commander",
    ],
    visibility = ["//actions:__subpackages__"],
    deps = [
        "//diagram/elk_to_svg",
    ],
)

bzd_nodejs_web_library(
    name = "library",
    srcs = [
        "bdl_to_elk.mjs",
        "library.mjs",
    ],
    library_name = "bdlToSvg",
    main = "library.mjs",
    deps = [
        "//diagram/elk_to_svg",
    ],
)

py_binary(
    name = "markdown",
    srcs = [
        "markdown.py",
    ],
)

doc_library(
    name = "doc_library",
    data = [
        ":library",
    ],
)
