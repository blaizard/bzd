load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = glob([
        "*.cc",
    ]),
    deps = [
        "//cc/bzd/algorithm",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//tools/cc_test",
    ],
)
