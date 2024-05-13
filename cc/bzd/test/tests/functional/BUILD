load("@rules_python//python:defs.bzl", "py_test")
load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "failure",
    srcs = [
        "failure.cc",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "success",
    srcs = [
        "success.cc",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd/test",
    ],
)

py_test(
    name = "test",
    srcs = [
        "test.py",
    ],
    data = [
        ":failure",
        ":success",
    ],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
