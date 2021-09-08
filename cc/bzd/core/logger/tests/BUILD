load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/logger",
        "//tools/cc_test",
    ],
) for path in glob([
    "*.cc",
])]
