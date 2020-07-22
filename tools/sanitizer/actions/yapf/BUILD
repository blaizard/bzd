load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "yapf",
    srcs = ["main.py"],
    main = "main.py",
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    data = [
        "yapf.ini",
    ],
    deps = [
        requirement("yapf"),
        "//tools/sanitizer/utils:workspace",
    ],
)
