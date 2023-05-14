load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/algorithm:equal",
        "//cc/bzd/container",
        "//cc/bzd/container/range:stream",
        "//cc/bzd/container/range/views",
        "//cc/bzd/test",
        "//cc/bzd/test/types:range",
    ],
) for path in glob([
    "*.cc",
])]
