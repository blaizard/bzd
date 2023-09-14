load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "mypy",
    srcs = ["main.py"],
    data = [
        "mypy.ini",
    ],
    main = "main.py",
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    deps = [
        requirement("mypy"),
        requirement("typing_extensions"),
        requirement("mypy_extensions"),
        "//tools/sanitizer/utils/python:workspace",
        "@bzd_python//bzd/utils:worker",
    ],
)
