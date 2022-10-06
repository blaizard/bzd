load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/test",
        "//cc/bzd/test/types:range",
        "//cc/bzd/container/range/views",
        "//cc/bzd/algorithm:equal"
    ],
) for path in glob([
    "*.cc",
])]
