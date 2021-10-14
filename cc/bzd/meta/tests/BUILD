load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/meta",
        "//cc/bzd/utility:move",
        "//tools/cc_test",
    ],
) for path in glob([
    "*.cc",
])]
