load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "yaml",
    command = "{binary} --config {config} --endswith '.yml' --endswith '.yaml' $@",
    locations = {
        ".prettierrc.json": "config",
        "//actions/prettier": "binary",
    },
    visibility = ["//visibility:public"],
)
