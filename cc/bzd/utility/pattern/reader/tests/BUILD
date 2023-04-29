load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "regexpr",
    srcs = [
        "regexpr.cc",
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern/reader:regexpr",
    ],
)

bzd_cc_test(
    name = "tests",
    srcs = [
        "integral.cc",
        "pattern.cc",
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern/reader:integral",
        "//cc/bzd/utility/pattern/reader:pattern",
    ],
)
