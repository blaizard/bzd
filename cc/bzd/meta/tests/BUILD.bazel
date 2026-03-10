load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/meta",
        "//cc/bzd/test",
        "//cc/bzd/test/types",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:move",
    ],
) for path in glob([
    "*.cc",
])]
