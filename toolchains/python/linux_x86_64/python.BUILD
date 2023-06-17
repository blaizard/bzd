load("@rules_python//python:defs.bzl", "py_runtime")

filegroup(
    name = "files",
    srcs = glob(
        include = [
            "bin/**",
            "extensions/**",
            "include/**",
            "lib/**",
            "libs/**",
            "share/**",
        ],
        exclude = [
            # Bazel does not support spaces in file names.
            "**/* *",
            # Unused shared libraries. `python` executable and the `:libpython` target
            # depend on `libpython{python_version}.so.1.0`.
            "lib/libpython*.so",
            # static libraries
            "lib/**/*.a",
            # tests for the standard libraries.
            "lib/python*/**/test/**",
            "lib/python*/**/tests/**",
        ]
    ),
)

py_runtime(
    name = "runtime",
    files = [":files"],
    interpreter = "bin/python3",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)
