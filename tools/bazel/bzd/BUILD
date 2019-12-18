py_library(
    name = "json5",
    srcs = glob([
        "json5/*.py",
    ]),
)

py_binary(
    name = "parser",
    srcs = glob([
        "*.py",
    ]),
    main = "parser.py",
    deps = [
        ":json5",
    ],
)
