load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "ssh",
    srcs = [
        "ssh.py",
    ],
    visibility = ["//targets/docker:__pkg__"],
    deps = [
        "@bzd_python//bzd/utils:ssh",
    ],
)
