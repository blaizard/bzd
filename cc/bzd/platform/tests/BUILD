load("//tools/bazel_build/rules:cc.bzl", "cc_test")

[cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/platform:system",
        "//tools/cc_test",
    ],
) for path in glob([
    "*.cc",
])]
