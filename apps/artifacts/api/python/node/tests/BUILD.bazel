load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "node",
    srcs = ["node_test.py"],
    main = "node_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "//apps/artifacts/api/python/node",
        "@bzd_python//bzd/http:client_mock",
    ],
)
