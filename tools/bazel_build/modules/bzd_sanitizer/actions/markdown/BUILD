load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "markdown",
    command = "{binary} --config {config} --endswith '.md' $@",
    locations = {
        ".prettierrc.json": "config",
        "@bzd_sanitizer//actions/prettier": "binary",
    },
    visibility = ["//visibility:public"],
)
