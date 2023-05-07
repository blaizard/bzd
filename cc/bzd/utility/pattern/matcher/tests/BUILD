load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "integral.cc",
        "pattern.cc",
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern/matcher:integral",
        "//cc/bzd/utility/pattern/matcher:output_stream",
        "//cc/bzd/utility/pattern/matcher:pattern",
    ],
)
