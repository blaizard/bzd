load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "generator",
    srcs = glob([
        "**/*.py",
    ]),
    main = "generator.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd/yaml",
    ],
)
