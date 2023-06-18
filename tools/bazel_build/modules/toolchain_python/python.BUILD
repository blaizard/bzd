load("@rules_python//python:defs.bzl", "py_runtime")
load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")

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

alias(
    name = "interpreter",
    actual = "bin/python3"
)

py_runtime(
    name = "runtime",
    files = [":files"],
    interpreter = ":interpreter",
    python_version = "PY3",
)

py_runtime_pair(
    name = "py_runtime_pair",
    py3_runtime = ":runtime",
)
