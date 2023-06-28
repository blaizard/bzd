load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "integral.cc",
        "pattern.cc",
    ],
    deps = [
        "//cc/bzd/container:ichannel_buffered",
        "//cc/bzd/test",
        "//cc/bzd/test/types:ichannel",
        "//cc/bzd/utility/pattern:from_stream",
    ],
)
