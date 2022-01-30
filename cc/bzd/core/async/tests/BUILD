load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "async",
    srcs = [
        "async.cc",
    ],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "cancellation",
    srcs = [
        "cancellation.cc",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd/core:delay",
        "//cc/bzd/core/async",
        "//cc/bzd/test",
    ],
)
