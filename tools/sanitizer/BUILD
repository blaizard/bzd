load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "sanitizer",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--action",
        "$(location //tools/sanitizer/actions/eslint)",
        "--action",
        "$(location //tools/sanitizer/actions/clang_format)",
    ],
    data = [
        "//tools/sanitizer/actions/clang_format",
        "//tools/sanitizer/actions/eslint",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd/utils:run",
        "//python/bzd:env",
    ],
)
