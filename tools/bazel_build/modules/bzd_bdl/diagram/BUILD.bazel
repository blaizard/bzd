load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_library")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_nodejs_web_library(
    name = "library",
    srcs = [
        "bdl_to_elk.mjs",
        "library.mjs",
    ],
    library_name = "BdlToSvg",
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
