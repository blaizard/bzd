load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/serialization/types:integral",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
