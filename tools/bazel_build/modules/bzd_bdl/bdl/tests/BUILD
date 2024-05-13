load("@rules_python//python:defs.bzl", "py_test")

[py_test(
    name = path.replace(".py", ""),
    srcs = [
        path,
    ],
    deps = [
        "//bdl:generators",
        "//bdl:grammar",
    ],
) for path in glob([
    "*.py",
])]
