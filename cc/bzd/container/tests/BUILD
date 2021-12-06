load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/core/assert",
        "//cc/bzd/meta:union",
        "//cc/bzd/test",
        "//cc/bzd/test:types",
        "//cc/bzd/test:utility",
        "//cc/bzd/type_traits:is_same",
    ],
) for path in glob([
    "*.cc",
])]
