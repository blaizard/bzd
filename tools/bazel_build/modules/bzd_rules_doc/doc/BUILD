load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

py_binary(
    name = "builder",
    srcs = [
        "builder.py",
    ],
    data = [
        ":mkdocs.yml.template",
    ],
    visibility = ["//visibility:public"],
)

py_console_script_binary(
    name = "mkdocs",
    pkg = "@pip//mkdocs",
    visibility = ["//visibility:public"],
    deps = [
        requirement("mkdocs"),
        requirement("mkdocs-material"),
        requirement("mkdocs-exclude"),
    ],
)

py_binary(
    name = "preprocessor",
    srcs = ["preprocessor.py"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "doxygen_parser",
    srcs = ["doxygen_parser.py"],
    visibility = ["//visibility:public"],
)
