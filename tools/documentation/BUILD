load("//tools/documentation:defs.bzl", "doc_binary")

doc_binary(
    name = "documentation",
    srcs = [
        "//include:bzd_files",
    ]
)

py_binary(
    name = "generator",
    main = "parsedocumentation.py",
    srcs = glob([
        "*.py",
    ]),
)
