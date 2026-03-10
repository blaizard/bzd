load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/container",
        "//cc/bzd/container:function_ref",
        "//cc/bzd/core/assert",
        "//cc/bzd/meta:union",
        "//cc/bzd/test",
        "//cc/bzd/test:utility",
        "//cc/bzd/test/types",
        "//cc/bzd/type_traits:is_same",
    ],
) for path in glob([
    "*.cc",
])]
