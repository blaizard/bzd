load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core/io",
        "//cc/bzd/test",
        "//cc/bzd/test:utility",
    ],
) for path in glob([
    "*.cc",
])]
