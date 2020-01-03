py_library(
    name = "json5",
    srcs = glob([
        "json5/*.py",
    ]),
    visibility = ["//visibility:private"],
)

py_library(
    name = "formats",
    srcs = glob([
        "formats/*.py",
    ]),
    visibility = ["//visibility:private"],
)

py_binary(
    name = "generator",
    srcs = glob([
        "*.py",
    ]),
    main = "generator.py",
    visibility = ["//visibility:public"],
    deps = [
        ":formats",
        ":json5",
    ],
)
