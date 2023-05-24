load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "regexp",
    srcs = [
        "regexp.cc",
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/regexp",
    ],
)

bzd_cc_test(
    name = "async",
    srcs = [
        "async.cc",
    ],
    deps = [
        "//cc/bzd/container:ichannel_buffered",
        "//cc/bzd/test",
        "//cc/bzd/test/types",
        "//cc/bzd/utility/regexp:async",
    ],
)
