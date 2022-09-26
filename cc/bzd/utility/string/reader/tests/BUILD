load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "from_string.cc",
        "integral.cc",
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/string/reader:from_string",
        "//cc/bzd/utility/string/reader:integral",
    ],
)
