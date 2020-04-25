load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "sanitizer",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--config=linux_x86_64_clang",
        "--clang-format",
        "%EXEC_ROOT%/$(location @linux_x86_64_clang_9_0_0//:clang_format)",
        "--eslint",
        "%EXEC_ROOT%/$(execpath //tools/sanitizer/eslint)",
    ],
    data = [
        "//tools/sanitizer/eslint",
        "@linux_x86_64_clang_9_0_0//:clang_format",
    ],
    main = "sanitizer.py",
)
