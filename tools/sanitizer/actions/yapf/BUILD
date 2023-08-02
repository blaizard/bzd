load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "yapf",
    srcs = ["main.py"],
    data = [
        "yapf.ini",
    ],
    main = "main.py",
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    deps = [
        requirement("yapf"),
        "//python/bzd/utils:worker",
        "//tools/sanitizer/utils/python:workspace",
    ],
)
