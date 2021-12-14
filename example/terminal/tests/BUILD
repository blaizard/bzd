load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "reader",
    srcs = [
        "reader.cc",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/generic/stream/stub",
        "//example/terminal:reader",
    ],
)
