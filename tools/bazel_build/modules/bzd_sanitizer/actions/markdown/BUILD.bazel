load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "markdown",
    command = "{binary} --config {config} --endswith '.md' $@",
    locations = {
        ".prettierrc.json": "config",
        "//actions/prettier": "binary",
    },
    visibility = ["//visibility:public"],
)
