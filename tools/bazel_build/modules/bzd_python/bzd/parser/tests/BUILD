load("@rules_python//python:defs.bzl", "py_test")

[py_test(
    name = path.replace(".py", ""),
    srcs = [
        path,
    ],
    deps = [
        "@bzd_python//bzd/parser",
        "@bzd_python//bzd/parser/tests/support",
    ],
) for path in glob([
    "*.py",
])]
