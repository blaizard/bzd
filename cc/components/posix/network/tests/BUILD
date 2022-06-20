load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "network.cc",
        "ipv4.cc",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/posix/network",
    ],
)
