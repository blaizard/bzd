load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/algorithm",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
