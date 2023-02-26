load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob(["*.cc"]),
    deps = [
        "//cc/bzd/test",
        "//cc/libs/timer",
        "//cc/libs/timer/tests/impl:timer",
    ],
)
