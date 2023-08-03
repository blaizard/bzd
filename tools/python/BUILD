load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

exports_files(["requirements.txt"])

sh_binary_wrapper(
    name = "yapf",
    command = "{binary} --config {config} $@",
    locations = {
        "@bzd_sanitizer//actions/yapf": "binary",
        "pyproject.toml": "config",
    },
    visibility = ["//visibility:public"],
)
