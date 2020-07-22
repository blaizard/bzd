load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

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
        "//tools/sanitizer/utils:workspace",
    ],
)
