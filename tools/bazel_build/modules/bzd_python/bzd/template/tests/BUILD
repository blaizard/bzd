load("@rules_python//python:defs.bzl", "py_test")

[py_test(
    name = path.replace(".py", ""),
    srcs = [
        path,
    ],
    data = glob([
        "support/*",
    ]),
    deps = [
        "//python/bzd/template",
    ],
) for path in glob([
    "*.py",
])]
