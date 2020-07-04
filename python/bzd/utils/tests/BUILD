load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "run",
    srcs = ["run_test.py"],
    main = "run_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd/utils:run",
    ],
)
