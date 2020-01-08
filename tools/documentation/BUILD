load("//tools/documentation:defs.bzl", "cc_documentation")

cc_documentation(
    name = "documentation",
    srcs = [
        "//src/bzd:headers",
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
