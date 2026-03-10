load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/type_traits",
    ],
) for path in glob([
    "*.cc",
])]
