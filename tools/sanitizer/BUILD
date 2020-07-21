load("@rules_python//python:python.bzl", "py_binary")

filegroup(
    name = "config",
    srcs = [
        ".sanitizer.json",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "sanitizer",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--action",
        "$(location //tools/sanitizer/actions/buildifier)",
        "--action",
        "$(location //tools/sanitizer/actions/eslint)",
        "--action",
        "$(location //tools/sanitizer/actions/clang_format)",
        "--action",
        "$(location //tools/sanitizer/actions/structure)",
        "--action",
        "$(location //tools/sanitizer/actions/autopep8)",
        "--action",
        "$(location //tools/sanitizer/actions/mypy)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/autopep8",
        "//tools/sanitizer/actions/buildifier",
        "//tools/sanitizer/actions/clang_format",
        "//tools/sanitizer/actions/eslint",
        "//tools/sanitizer/actions/mypy",
        "//tools/sanitizer/actions/structure",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
    ],
)
