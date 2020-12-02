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
        "$(location //tools/sanitizer/actions/buildifier)",
        "--action",
        "$(location //tools/sanitizer/actions/eslint)",
        "--action",
        "$(location //tools/sanitizer/actions/structure)",
        "--action",
        "$(location //tools/sanitizer/actions/mypy)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/buildifier",
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
        "$(location //tools/sanitizer/actions/clang_format)",
        "--action",
        "$(location //tools/sanitizer/actions/yapf)",
        "--action",
        "$(location //tools/sanitizer/actions/files)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/clang_format",
        "//tools/sanitizer/actions/files",
        "//tools/sanitizer/actions/prettier",
        "//tools/sanitizer/actions/yapf",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
    ],
)
