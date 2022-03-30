load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container:tuple",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/utility",
    ],
) for path in glob([
    "*.cc",
])]
