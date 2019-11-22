load("//tools/documentation:defs.bzl", "cc_documentation")

cc_documentation(
    name = "documentation",
    srcs = [
        "//include:bzd_files",
    ],
    tags = ["manual"],
    data = glob([
        "**/*.py",
    ]),
)

py_binary(
    name = "generator",
    main = "documentation.py",
    srcs = glob([
        "**/*.py",
    ]),
)
