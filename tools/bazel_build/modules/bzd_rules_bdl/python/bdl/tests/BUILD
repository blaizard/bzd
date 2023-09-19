load("@rules_python//python:defs.bzl", "py_test")

[py_test(
    name = path.replace(".py", ""),
    srcs = [
        path,
    ],
    deps = [
        "//tools/bdl:generators",
        "//tools/bdl:grammar",
    ],
) for path in glob([
    "*.py",
])]
