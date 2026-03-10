load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
