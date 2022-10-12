load("@rules_python//python:python.bzl", "py_binary")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "clang_format",
    binary = ":binary",
    command = """
{binary} --clang-format $(location //toolchains/cc/linux_x86_64_clang:clang_format) $@
    """,
    data = [
        "//toolchains/cc/linux_x86_64_clang:clang_format",
    ],
    visibility = ["//tools/sanitizer:__pkg__"],
)

py_binary(
    name = "binary",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--clang-format",
        "$(location //toolchains/cc/linux_x86_64_clang:clang_format)",
    ],
    data = [
        "//toolchains/cc/linux_x86_64_clang:clang_format",
    ],
    main = "clang_format.py",
    deps = [
        "//python/bzd/utils:run",
        "//tools/sanitizer/utils/python:workspace",
    ],
)
