load("@rules_python//python:defs.bzl", "py_runtime")

py_runtime(
    name = "runtime",
    files = glob(["usr/lib/**/*"]) + ["@coveragepy//:all"],
    interpreter = "usr/bin/python3.8",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)
