load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob(["*.cc"]),
    tags = ["manual"],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/test/types:ichannel",
        "//cc/bzd/test/types:istream_reader",
        "//cc/libs/reader:stream_reader",
    ],
)
