load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob(["*.cc"]),
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/test/types:istream",
        "//cc/bzd/test/types:istream_reader",
        "//cc/libs/reader:stream_reader",
    ],
)
