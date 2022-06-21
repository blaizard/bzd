load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "ipv4.cc",
        "network.cc",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/posix/network",
    ],
)
