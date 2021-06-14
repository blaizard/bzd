load("@rules_python//python:defs.bzl", "py_test")

[py_test(
    name = path.replace(".py", ""),
    srcs = [
        path,
    ],
    deps = [
        "//python/bzd/template2",
    ],
) for path in glob([
    "*.py",
])]
