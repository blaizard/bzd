load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "sanitizer",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    main = "sanitizer.py",
    args = [
        "--clang-format", "%EXEC_ROOT%/$(location @linux_x86_64_clang_9_0_0//:clang_format)"
    ],
    data = [
        "@linux_x86_64_clang_9_0_0//:clang_format"
    ]
)
