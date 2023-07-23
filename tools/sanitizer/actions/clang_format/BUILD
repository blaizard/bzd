load("@rules_python//python:python.bzl", "py_binary")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "clang_format",
    locations = {
        ":binary": "binary",
        "@clang//:clang_format": "clang_format",
    },
    command = """
{binary} --clang-format {clang_format} $@
    """,
    visibility = ["//tools/sanitizer:__pkg__"],
)

py_binary(
    name = "binary",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    main = "clang_format.py",
    deps = [
        "//python/bzd/utils:run",
        "//tools/sanitizer/utils/python:workspace",
    ],
)
