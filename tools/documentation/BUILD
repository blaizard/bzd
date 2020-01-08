load("//tools/documentation:defs.bzl", "cc_documentation")

cc_documentation(
    name = "documentation",
    srcs = [
        "//platform:documentation",
    ],
    data = glob([
        "**/*.py",
    ]),
    tags = ["manual"],
)

py_binary(
    name = "generator",
    srcs = glob([
        "**/*.py",
    ]),
    main = "documentation.py",
)
