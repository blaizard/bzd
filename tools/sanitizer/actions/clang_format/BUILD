load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "clang_format",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--clang-format",
        "$(location @linux_x86_64_clang_9_0_0//:clang_format)",
    ],
    data = [
        "@linux_x86_64_clang_9_0_0//:clang_format",
    ],
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//python/bzd/utils:run",
        "//tools/sanitizer/utils:workspace",
    ],
)
