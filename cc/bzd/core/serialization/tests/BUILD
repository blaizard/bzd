load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/serialization/types:floating_point",
        "//cc/bzd/core/serialization/types:integral",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
