load("@rules_python//python:defs.bzl", "py_runtime")

py_runtime(
    name = "runtime",
    files = glob(["lib/**/*.py", "lib/**/*.so.*"]) + ["@coveragepy//:all"],
    interpreter = "bin/python3.8",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)
