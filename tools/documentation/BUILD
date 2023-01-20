load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "mkdocs_wrapper",
    srcs = [
        "mkdocs_wrapper.py"
    ],
    deps = [
        requirement("mkdocs"),
        requirement("mkdocs-material"),
        requirement("mkdocs-exclude")
    ],
    visibility = [
        "//docs:__pkg__",
    ],
)
