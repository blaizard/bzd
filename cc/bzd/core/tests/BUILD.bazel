load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/core",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
