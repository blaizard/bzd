load("@rules_python//python:defs.bzl", "py_runtime")

py_runtime(
    name = "runtime",
    files = glob(["usr/lib/**/*"]),
    interpreter = "usr/bin/python3.6",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)
