load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "mkdocs_wrapper",
    srcs = [
        "mkdocs_wrapper.py",
    ],
    visibility = [
        "//docs:__pkg__",
    ],
    deps = [
        requirement("mkdocs"),
        requirement("mkdocs-material"),
        requirement("mkdocs-exclude"),
    ],
)
