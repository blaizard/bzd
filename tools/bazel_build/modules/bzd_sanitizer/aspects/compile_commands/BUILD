load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "client",
    srcs = [
        "client.py",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "server",
    srcs = [
        "server.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
