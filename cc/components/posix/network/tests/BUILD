load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "network.cc",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd/test",
        "//cc/components/posix/network",
    ],
)
