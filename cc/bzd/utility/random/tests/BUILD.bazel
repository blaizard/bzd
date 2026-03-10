load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/test",
        "//cc/bzd/utility/random",
    ],
) for path in glob([
    "*.cc",
])]
