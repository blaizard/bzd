load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@rules_python//python:defs.bzl", "py_library")

bzd_config(
    name = "config",
    include_workspace_status = ["STABLE_VERSION"],
    visibility = ["//visibility:private"],
)

py_library(
    name = "release",
    srcs = [
        "release.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":config",
        "@bzd_python//bzd/http:client",
    ],
)
