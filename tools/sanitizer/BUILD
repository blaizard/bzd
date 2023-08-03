load("@rules_python//python:python.bzl", "py_binary")

filegroup(
    name = "config",
    srcs = [
        ".sanitizer.json",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "analyzer",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--action",
        "$(location //tools/sanitizer/actions/eslint)",
        "--action",
        "$(location //tools/sanitizer/actions/mypy)",
        "--action",
        "$(location //tools/sanitizer/actions/shell)",
        "--action",
        "$(location //tools/sanitizer/actions/ci)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/ci",
        "//tools/sanitizer/actions/eslint",
        "//tools/sanitizer/actions/mypy",
        "//tools/sanitizer/actions/shell",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
    ],
)

py_binary(
    name = "formatter",
    srcs = glob(
        [
            "*.py",
        ],
    ),
    args = [
        "--action",
        "$(location //tools/sanitizer/actions/prettier)",
        "--action",
        "$(location //tools/sanitizer/actions/files)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/files",
        "//tools/sanitizer/actions/prettier",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
    ],
)
