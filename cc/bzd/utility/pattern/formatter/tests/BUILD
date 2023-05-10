load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "format.cc",
        "integral.cc",
    ],
    deps = [
        "//cc/bzd/container:string_stream",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern/formatter:to_stream",
        "//cc/bzd/utility/pattern/formatter:to_string",
    ],
)
