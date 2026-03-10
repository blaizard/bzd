load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "build",
    srcs = [
        "build.py",
    ],
    data = [
        ":project",
    ],
    deps = [
        "@bzd_python//bzd/command_extractor:gcc",
        "@bzd_python//bzd/utils:run",
    ],
)

filegroup(
    name = "project",
    srcs = glob(["project/**"]),
)
