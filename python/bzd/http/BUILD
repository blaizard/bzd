load("@bzd_python_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "client",
    srcs = ["client.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("certifi"),
    ],
)

py_library(
    name = "server",
    srcs = ["server.py"],
    visibility = ["//visibility:public"],
)
