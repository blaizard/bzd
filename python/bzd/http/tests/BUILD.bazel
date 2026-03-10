load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "client",
    srcs = ["client_test.py"],
    main = "client_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "//bzd/http:client_mock",
    ],
)
