load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "prettier",
    command = "{binary} --config {config} --endswith '.mjs' --endswith '.cjs' --endswith '.js' --endswith '.vue' --endswith '.css' --endswith '.scss' $@",
    locations = {
        ".prettierrc.json": "config",
        "@bzd_sanitizer//actions/prettier": "binary",
    },
    visibility = ["//visibility:public"],
)
