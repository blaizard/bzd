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
        # Disabled because of this: https://github.com/eslint/eslint/discussions/15305
        #"--action",
        #"$(location //tools/sanitizer/actions/eslint)",
        "--action",
        "$(location //tools/sanitizer/actions/mypy)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/eslint",
        "//tools/sanitizer/actions/mypy",
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
        "$(location //tools/sanitizer/actions/files)",
    ],
    data = [
        ":config",
        "//tools/sanitizer/actions/files",
    ],
    main = "sanitizer.py",
    deps = [
        "//python/bzd:env",
        "//python/bzd/utils:run",
    ],
)
