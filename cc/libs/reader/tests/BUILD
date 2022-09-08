load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob(["*.cc"]),
    deps = [
        "//cc/libs/reader:stream_reader",
        "//cc/bzd/test",
        "//cc/bzd/test/types:istream",
        "//cc/bzd/test/types:istream_reader"
    ]
)
