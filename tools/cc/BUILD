load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

exports_files([".clang-tidy"])

sh_binary_wrapper(
    name = "clang_format",
    command = "{binary} --clang-format {clang_format} --config {config} --cc $@",
    locations = {
        ".clang-format": "config",
        "@bzd_sanitizer//actions/clang_format": "binary",
        "@clang//:clang_format": "clang_format",
    },
    visibility = ["//visibility:public"],
)
