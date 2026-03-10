load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/math",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
