load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "autopep8",
    srcs = ["main.py"],
    main = "main.py",
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    deps = [
        requirement("autopep8"),
        "//tools/sanitizer/utils:workspace",
    ],
)
