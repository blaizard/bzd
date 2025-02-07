load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

exports_files(
    ["js/script.js"],
    visibility = ["//:__subpackages__"],
)

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
