load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

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
        requirement("typed-ast"),
        "//python/bzd/utils:worker",
        "//tools/sanitizer/utils/python:workspace",
    ],
)
