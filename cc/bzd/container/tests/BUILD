load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = glob([
        "*.cc",
    ]),
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/meta:union",
        "//tools/cc_test",
    ],
)
