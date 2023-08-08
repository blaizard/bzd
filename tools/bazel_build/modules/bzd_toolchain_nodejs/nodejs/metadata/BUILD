load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "package_json_template",
    srcs = ["package.json.template"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "webpack_config_template",
    srcs = ["webpack.config.cjs.template"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "metadata_json",
    srcs = ["metadata.json"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "metadata",
    srcs = ["metadata.py"],
    visibility = ["//visibility:public"],
)
