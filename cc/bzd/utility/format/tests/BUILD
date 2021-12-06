load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob([
        "*.cc",
    ]),
    deps = [
        "//cc/bzd/container:string_stream",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/utility/format",
    ],
)
