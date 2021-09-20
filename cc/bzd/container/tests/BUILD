load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/core:assert",
        "//cc/bzd/meta:union",
        "//cc/bzd/type_traits:is_same",
        "//tools/cc_test",
    ],
) for path in glob([
    "*.cc",
])]
