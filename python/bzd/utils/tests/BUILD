load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "run",
    main = "run_test.py",
    srcs = ["run_test.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd/utils:run",
    ],
)
