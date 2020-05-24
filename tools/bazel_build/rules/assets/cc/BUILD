load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "metadata",
    srcs = ["metadata.py"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "metadata_json",
    srcs = ["metadata.json"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "profiler",
    srcs = ["profiler.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyelftools"),
    ],
)
